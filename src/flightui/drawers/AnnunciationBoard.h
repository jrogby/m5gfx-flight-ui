#pragma once
#include <M5Unified.hpp>
#include "flightui/types.h"
#include "flightui/color.h"
#include "flightui/fonts/vlw.h"
#include "Ecam.h"
#include "IconLoader.h"
#include "LampIconStyle.h"

namespace ui {

// Annunciation board statuses
enum LampStatus : uint8_t {STATUS_NA, STATUS_ERROR, STATUS_WARNING, STATUS_OK, STATUS_PENDING, STATUS_COUNT};

// Predefined annunciation board layouts
struct BoardLayout {
    u16 cols;
    u16 rows;
    const uint8_t* const* font; // VLW font slot (filled at boot, see fonts/slots.h)
};

// Common layouts for a canvas with width 320px
// w 159px
constexpr BoardLayout LAYOUT_2x2 = {2, 2, &ECAMFontRegular34};  // h 159
constexpr BoardLayout LAYOUT_2x3 = {2, 3, &ECAMFontRegular32};  // h  78
constexpr BoardLayout LAYOUT_2x4 = {2, 4, &ECAMFontRegular28};  // h  58 (51)
constexpr BoardLayout LAYOUT_2x5 = {2, 5, &ECAMFontRegular24};  // h  46
constexpr BoardLayout LAYOUT_2x6 = {2, 6, &ECAMFontRegular22};  // h  35
constexpr BoardLayout LAYOUT_2x8 = {2, 8, &ECAMFontRegular18};  // h  28 (25) :))

// w 105px
constexpr BoardLayout LAYOUT_3x2 = {3, 2, &ECAMFontRegular22};  // h 104 - icon (32) above the label

// w 78px
constexpr BoardLayout LAYOUT_4x3 = {4, 3, &ECAMFontRegular19};  // h  78 (68)
constexpr BoardLayout LAYOUT_4x4 = {4, 4, &ECAMFontRegular19};  // h  58 (51)
constexpr BoardLayout LAYOUT_4x5 = {4, 5, &ECAMFontRegular19};  // h  46 (40) :)
constexpr BoardLayout LAYOUT_4x8 = {4, 8, &ECAMFontRegular18};  // h  28 (25)

// w 62px
constexpr BoardLayout LAYOUT_5x2 = {5, 2, &ECAMFontRegular17};  // h 119 (103) :(
constexpr BoardLayout LAYOUT_5x3 = {5, 3, &ECAMFontRegular19};  // h  78 (68) ok
constexpr BoardLayout LAYOUT_5x4 = {5, 4, &ECAMFontRegular18};  // h  62 (51)
constexpr BoardLayout LAYOUT_5x5 = {5, 5, &ECAMFontRegular17};  // h  46 (40)
constexpr BoardLayout LAYOUT_5x8 = {5, 8, &ECAMFontRegular12};  // h  28

// w 38px
constexpr BoardLayout LAYOUT_8x8 = {8, 8, &ECAMFontRegular12};  // w  38  h  38

static constexpr uint8_t MAX_LAMPS = 32;

// Lamp gradient colors per status: {top, bottom (dark)}
inline u16 lampColor(LampStatus s, bool dark) {
    switch (s) {
        case STATUS_ERROR:   return dark ? color::LAMP_ERROR_DK   : color::LAMP_ERROR;
        case STATUS_WARNING: return dark ? color::LAMP_WARNING_DK : color::LAMP_WARNING;
        case STATUS_OK:      return dark ? color::LAMP_OK_DK      : color::LAMP_OK;
        case STATUS_PENDING: return dark ? color::LAMP_PENDING_DK : color::LAMP_PENDING;
        default:             return dark ? color::LAMP_NA_DK      : color::LAMP_NA;
    }
}

// Icon size that fits a lamp: largest of 32/24/16 with >= 4 px margin top/bottom and room for a label
// (beside it: w >= 3 x icon; tall lamps put the icon above the label, see drawLampFast)
inline uint8_t lampIconSize(u16 lamp_w, u16 lamp_h) {
    for (uint8_t s : {32, 24, 16})
        if (s + 8 <= lamp_h && s * 3 <= lamp_w) return s;
    return 0;
}

// Pre-rendered background sprite cache per status color
struct LampBGCache {
    M5Canvas sprites[STATUS_COUNT];
    bool ready = false;
    u16 cw = 0, ch = 0;

    // Shared by all boards with the same lamp size, kept after the board is destroyed:
    // the 5 PSRAM gradient fills cost most of a status page's setup on every swipe-in
    static LampBGCache* shared(M5Canvas &parent, u16 w, u16 h) {
        static LampBGCache caches[2];  // 2x6 status pages + one other layout
        for (auto &c : caches) if (c.ready && c.cw == w && c.ch == h) return &c;
        for (auto &c : caches) if (!c.ready) { c.init(parent, w, h); return &c; }
        caches[1].destroy(); caches[1].init(parent, w, h);  // full: reuse the second slot
        return &caches[1];
    }

    void init(M5Canvas &parent, u16 w, u16 h) {
        if (ready) return;
        cw = w; ch = h;
        const u16 colors[][2] = {
            {color::LAMP_NA,      color::LAMP_NA_DK},
            {color::LAMP_ERROR,   color::LAMP_ERROR_DK},
            {color::LAMP_WARNING, color::LAMP_WARNING_DK},
            {color::LAMP_OK,      color::LAMP_OK_DK},
            {color::LAMP_PENDING, color::LAMP_PENDING_DK},
        };
        for (uint8_t i = 0; i < STATUS_COUNT; i++) {
            sprites[i].setColorDepth(parent.getColorDepth());
            sprites[i].setPsram(true);  // 5 lamp-sized sprites per board - keep out of internal DRAM
            sprites[i].createSprite(w, h);
            sprites[i].fillGradientRect(0, 0, w, h, colors[i][0], colors[i][1]);
        }
        ready = true;
    }

    void destroy() {
        if (!ready) return;
        for (uint8_t i = 0; i < STATUS_COUNT; i++)
            sprites[i].deleteSprite();
        ready = false;
    }
};

// Dirty-tracked annunciation board
struct AnnunciationBoard {
    M5Canvas *canvas = nullptr;
    LampBGCache *bg_cache = nullptr;  // shared (LampBGCache::shared), not owned
    const BoardLayout *layout = nullptr;
    bool draw_grid = true;
    uint8_t total_lamps = 0;
    u16 lamp_w = 0, lamp_h = 0;
    uint8_t icon_size = 0;               // from lamp size (lampIconSize), 0 = lamps too small
    LampIconStyle icon_style = ICON_LABEL;

    // Per-lamp dirty state
    LampStatus statuses[MAX_LAMPS];
    String labels[MAX_LAMPS];
    const char* icons[MAX_LAMPS] = {};   // icon name (<icon_dir>/<size>/<name>.png), nullptr = none
    LampStatus prev_statuses[MAX_LAMPS];
    String prev_labels[MAX_LAMPS];
    const char* prev_icons[MAX_LAMPS] = {};

    void create(M5Canvas &parent, uint8_t num_lamps, const String init_labels[],
                u16 width, u16 height, const BoardLayout& lay, bool grid = true) {
        layout = &lay;
        draw_grid = grid;
        total_lamps = min(num_lamps, MAX_LAMPS);

        canvas = new M5Canvas(&parent);
        canvas->setPsram(true); // Enable PSRAM for the canvas
        canvas->createSprite(width, height);

        if (grid) canvas->fillScreen(color::ECAM_BG);

        const u16 gw = grid ? 2 : 0;
        lamp_w = (width - (gw * (layout->cols - 1))) / layout->cols;
        lamp_h = (height - (layout->rows - 1)) / layout->rows;
        icon_size = lampIconSize(lamp_w, lamp_h);

        bg_cache = LampBGCache::shared(*canvas, lamp_w, lamp_h);

        // Init all lamps as NA
        canvas->loadFont(*layout->font);
        canvas->setTextDatum(MC_DATUM);
        canvas->setTextColor(color::LAMP_LABEL);
        for (uint8_t i = 0; i < total_lamps; i++) {
            statuses[i] = STATUS_NA;
            prev_statuses[i] = STATUS_NA;
            labels[i] = init_labels ? init_labels[i] : "";
            prev_labels[i] = labels[i];
            icons[i] = prev_icons[i] = nullptr;
            drawLampFast(i);
        }
    }

    // Set lamp state — actual draw deferred to flush(). icon: name of a tinted icon (static string),
    // drawn left of the label when the lamp is big enough and the label still fits.
    void set(uint8_t lamp_num, LampStatus status, const String& label, const char* icon = nullptr) {
        if (lamp_num >= total_lamps) return;
        statuses[lamp_num] = status;
        labels[lamp_num] = label;
        icons[lamp_num] = icon;
    }

    // Change icon color variant; redraws every lamp on the next flush()
    void setIconStyle(LampIconStyle style) {
        if (style == icon_style) return;
        icon_style = style;
        for (uint8_t i = 0; i < total_lamps; i++) prev_statuses[i] = STATUS_COUNT;  // force redraw
    }

    // Redraw only changed lamps
    void flush() {
        bool font_loaded = false;
        for (uint8_t i = 0; i < total_lamps; i++) {
            if (statuses[i] != prev_statuses[i] || labels[i] != prev_labels[i] || icons[i] != prev_icons[i]) {
                if (!font_loaded) {
                    canvas->loadFont(*layout->font);
                    canvas->setTextDatum(MC_DATUM);
                    canvas->setTextColor(color::LAMP_LABEL);
                    font_loaded = true;
                }
                drawLampFast(i);
                prev_statuses[i] = statuses[i];
                prev_labels[i] = labels[i];
                prev_icons[i] = icons[i];
            }
        }
    }

    void pushSprite(int x, int y) {
        if (canvas) canvas->pushSprite(x, y);
    }

    void destroy() {
        bg_cache = nullptr;  // shared cache stays
        if (canvas) { canvas->deleteSprite(); delete canvas; canvas = nullptr; }
    }

private:
    void drawLampFast(uint8_t lamp_num) {
        const u16 gw = draw_grid ? 2 : 0;
        const u16 x = (lamp_num % layout->cols) * (lamp_w + gw);
        const u16 y = (lamp_num / layout->cols) * (lamp_h + gw);

        // Blit pre-rendered gradient background
        bg_cache->sprites[statuses[lamp_num]].pushSprite(canvas, x, y);

        // Icon left (square margin), label centered in the rest. No icon if the label would not fit.
        if (icons[lamp_num] && icon_size && icon_style != ICON_OFF) {
            const u16 pad = (lamp_h - icon_size) / 2;
            const u16 text_x0 = pad + icon_size;          // text area starts after the icon
            const u16 text_w = lamp_w - text_x0 - pad / 2;
            const int16_t font_h = canvas->fontHeight();
            const bool stacked = lamp_h >= icon_size + font_h + 16;  // tall lamp: icon above the label
            if (stacked || canvas->textWidth(labels[lamp_num]) <= text_w) {
                const IconMask* m = icon_loader.mask(icons[lamp_num], icon_size);
                if (m) {
                    u16 c = color::LAMP_LABEL;
                    uint8_t opacity = 255;
                    switch (icon_style) {
                        case ICON_DARK:  c = lampColor(statuses[lamp_num], true); break;
                        case ICON_WHITE: c = TFT_WHITE; break;
                        case ICON_GHOST: opacity = 115; break;
                        default: break;
                    }
                    if (stacked) {  // icon + gap + label centered as one block
                        const u16 gap = 6;
                        const u16 top = y + (lamp_h - (icon_size + gap + font_h)) / 2;
                        IconLoader::drawMask(*canvas, m, x + (lamp_w - icon_size) / 2, top, c, opacity);
                        canvas->drawString(labels[lamp_num], x + lamp_w / 2, top + icon_size + gap + font_h / 2);
                        return;
                    }
                    IconLoader::drawMask(*canvas, m, x + pad, y + pad, c, opacity);
                    canvas->drawString(labels[lamp_num], x + text_x0 + text_w / 2, y + lamp_h / 2);
                    return;
                }
            }
        }

        // Draw label centered on lamp
        canvas->drawString(labels[lamp_num], x + lamp_w / 2, y + lamp_h / 2);
    }
};

// ---- Legacy API (wraps AnnunciationBoard for backward compat) ----

/**
 * Function to draw a lamp on the main canvas (legacy).
 */
void inline drawLamp(M5Canvas &c, uint lamp_num, LampStatus status, const String& label,
                    const BoardLayout& layout, bool draw_grid = true, bool load_font = false) {
    const u16 GRID_WIDTH = draw_grid ? 2 : 0;
    const u16 lamp_width = (c.width() - (GRID_WIDTH * (layout.cols - 1))) / layout.cols;
    const u16 lamp_height = (c.height() - (layout.rows - 1)) / layout.rows;
    const u16 x = (lamp_num % layout.cols) * (lamp_width + GRID_WIDTH);
    const u16 y = (lamp_num / layout.cols) * (lamp_height + GRID_WIDTH);

    u16 bg_color, bg_dark;
    switch (status) {
        case STATUS_NA:       bg_color = color::LAMP_NA;      bg_dark = color::LAMP_NA_DK; break;
        case STATUS_ERROR:    bg_color = color::LAMP_ERROR;   bg_dark = color::LAMP_ERROR_DK; break;
        case STATUS_WARNING:  bg_color = color::LAMP_WARNING; bg_dark = color::LAMP_WARNING_DK; break;
        case STATUS_OK:       bg_color = color::LAMP_OK;      bg_dark = color::LAMP_OK_DK; break;
        case STATUS_PENDING:  bg_color = color::LAMP_PENDING; bg_dark = color::LAMP_PENDING_DK; break;
        default:              bg_color = color::LAMP_NA;      bg_dark = color::LAMP_NA_DK; break;
    }

    c.fillGradientRect(x, y, lamp_width, lamp_height, bg_color, bg_dark);
    c.setTextDatum(MC_DATUM);
    if (load_font) c.loadFont(*layout.font);
    c.setTextColor(color::LAMP_LABEL);
    c.drawString(label, x + lamp_width/2, y + lamp_height/2);
}

inline void createAnnunciationBoard(M5Canvas &c,
    uint total_lamps,
    const String labels[],
    u16 width = 320, u16 height = 240, const BoardLayout& layout = LAYOUT_2x8,
    bool draw_grid = true)
{
    c.createSprite(width, height);
    if (draw_grid) c.fillScreen(color::ECAM_BG);
    c.loadFont(*layout.font);

    for (uint i = 0; i < total_lamps; i++) {
        drawLamp(c, i, STATUS_NA,
                labels ? labels[i] : "",
                layout, draw_grid, false);
    }
}

void inline updateLampStatus(M5Canvas &c,
    uint lamp_num,
    const String& label,
    LampStatus new_status, const BoardLayout& layout = LAYOUT_2x8,
    bool draw_grid = true)
{
    drawLamp(c, lamp_num, new_status, label, layout, draw_grid, true);
}

} // namespace ui