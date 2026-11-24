#pragma once
#include <initializer_list>
#include <M5GFX.h>

namespace ui::modal {

// Colors of a modal. Defaults = neutral info screen (white title, cyan status, yellow hints on black).
struct Style {
  uint16_t bg       = TFT_BLACK;
  uint16_t title_fg = TFT_WHITE;
  uint16_t title_bg = TFT_BLACK;   // box behind title text (e.g. TFT_RED for alerts)
  uint16_t status   = TFT_CYAN;
  uint16_t lines    = TFT_YELLOW;
  uint16_t bar      = TFT_GREEN;
};

// Presets
inline Style info()  { return Style{}; }
inline Style alert() { Style s; s.title_bg = TFT_RED; return s; }                 // white on red title
inline Style fatal() { Style s; s.title_fg = TFT_RED; s.status = TFT_RED; return s; }

/**
 * Full-screen blocking message (restart, OTA, fatal error). Drawn into the canvas, then pushed
 * to the LCD in one go (double buffered, no flicker). Built-in fonts only - no filesystem needed.
 *
 *   title        large, y=60
 *   status       Font4, y=110 (nullptr = none)
 *   progress_pct 0..100 draws bar + percentage at y=145, -1 = no bar
 *   lines        Font2, from y=140, 20 px apart (e.g. hints)
 */
inline void show(M5Canvas& buf, const char* title, const char* status = nullptr, int progress_pct = -1,
                 std::initializer_list<const char*> lines = {}, const Style& st = Style{}) {
  const int cx = buf.width() / 2;
  buf.fillScreen(st.bg);
  buf.setTextDatum(MC_DATUM);

  buf.setTextColor(st.title_fg, st.title_bg);
  buf.drawString(title, cx, 60, &fonts::Orbitron_Light_24);

  if (status) {
    buf.setTextColor(st.status, st.bg);
    buf.drawString(status, cx, 110, &fonts::Font4);
  }

  int y = 140;
  buf.setTextColor(st.lines, st.bg);
  for (const char* line : lines) {
    buf.drawString(line, cx, y, &fonts::Font2);
    y += 20;
  }

  if (progress_pct >= 0) {
    const int bar_x = 40;
    const int bar_y = 145;
    const int bar_w = buf.width() - 80;
    const int bar_h = 16;
    buf.drawRect(bar_x, bar_y, bar_w, bar_h, TFT_DARKGRAY);
    int fill_w = (bar_w - 2) * progress_pct / 100;
    if (fill_w > 0)
      buf.fillRect(bar_x + 1, bar_y + 1, fill_w, bar_h - 2, st.bar);

    char pct[8];
    snprintf(pct, sizeof(pct), "%d%%", progress_pct);
    buf.setTextColor(st.title_fg, st.bg);
    buf.drawString(pct, cx, bar_y + bar_h + 20, &fonts::Font4);
  }
  buf.pushSprite(0, 0);
}

}  // namespace ui::modal
