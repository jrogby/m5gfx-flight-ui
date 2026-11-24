#pragma once
#include <M5Unified.hpp>
#include <FS.h>
#include <map>
#include <vector>
#include <esp_heap_caps.h>
#include "flightui/log.h"
#include "flightui/interfaces.h"

namespace ui {

static constexpr const char *TAG_ICON = "IconLoader";

// Single icon sprite with metadata
struct Icon {
  M5Canvas sprite;
  bool loaded = false;

  void init(M5Canvas* parent) {
    if (!loaded) {
      sprite.setColorDepth(parent->getColorDepth());
    }
  }

  void destroy() {
    if (loaded) {
      sprite.deleteSprite();
      loaded = false;
    }
  }
};

/**
 * Tintable icon: 8-bit alpha mask (0 = transparent, 255 = opaque), decoded from a PNG.
 * Icons are white line art on transparent (data/icon/png/<size>/<name>.png); the color is chosen
 * per draw. Mask lives in PSRAM (32x32 = 1 kB), loaded on first use and cached.
 */
struct IconMask {
  uint8_t w = 0, h = 0;
  uint8_t *a8 = nullptr;  // nullptr = load failed (cached so a missing file is tried/logged once)
};

// Animation frame with timing
struct AnimFrame {
  M5Canvas sprite;
  uint16_t delay_ms;
};

class IconLoader {
private:
  std::map<String, Icon> icon_cache;
  std::map<String, IconMask> mask_cache;
  fs::FS* _fs = nullptr;  // app owns mount; lib only reads
  String _icon_dir = "/icon/png";  // tinted icons: <dir>/<size>/<name>.png

  // Read a whole file into a heap buffer (PSRAM if available). Caller frees.
  uint8_t* readFile(const char* path, size_t& len) {
    len = 0;
    if (!_fs) { FUI_LOGE("IconLoader not initialized (no fs): %s", path); return nullptr; }
    File file = _fs->open(path, "r");
    if (!file) { FUI_LOGE("Failed to open icon: %s", path); return nullptr; }
    len = file.size();
    uint8_t* buf = (uint8_t*)heap_caps_malloc(len, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) buf = (uint8_t*)malloc(len);
    if (buf) file.read(buf, len);
    file.close();
    return buf;
  }

public:
  IconLoader() {}

  // icon_dir: base folder of the tinted PNG icons (<icon_dir>/<size>/<name>.png)
  void init(fs::FS& fs, const char* icon_dir = "/icon/png") { _fs = &fs; _icon_dir = icon_dir; }

  // ---- Tinted icons (PNG -> alpha mask) ----

  // Load a PNG as alpha mask (cached). The PNG is drawn on black: brightness = alpha, so white
  // line icons give their exact alpha. Returns nullptr if missing/invalid (logged once).
  const IconMask* loadMask(const char* path) {
    String key(path);
    auto it = mask_cache.find(key);
    if (it != mask_cache.end()) return it->second.a8 ? &it->second : nullptr;

    IconMask& m = mask_cache[key];  // failed entry stays (a8 = nullptr): no FS read every frame
    size_t len;
    uint8_t* buf = readFile(path, len);
    if (!buf) return nullptr;

    // PNG: 8-byte signature, IHDR width/height big-endian at offset 16/20
    static const uint8_t SIG[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    uint32_t w = 0, h = 0;
    if (len >= 24 && memcmp(buf, SIG, 8) == 0) {
      w = (buf[16] << 24) | (buf[17] << 16) | (buf[18] << 8) | buf[19];
      h = (buf[20] << 24) | (buf[21] << 16) | (buf[22] << 8) | buf[23];
    }
    if (w == 0 || h == 0 || w > 64 || h > 64) {
      FUI_LOGE("Not a PNG icon (max 64x64): %s", path);
      free(buf);
      return nullptr;
    }

    M5Canvas tmp;  // 24-bit: full 8-bit alpha resolution
    tmp.setColorDepth(24);
    tmp.setPsram(true);  // <4 KB: plain malloc would use internal DRAM
    bool ok = tmp.createSprite(w, h) != nullptr;
    if (ok) { tmp.fillSprite(TFT_BLACK); ok = tmp.drawPng(buf, len, 0, 0); }
    free(buf);
    if (ok) {
      m.a8 = (uint8_t*)heap_caps_malloc(w * h, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
      if (!m.a8) m.a8 = (uint8_t*)malloc(w * h);
      ok = m.a8 != nullptr;
    }
    if (ok) {
      m.w = w; m.h = h;
      for (uint32_t y = 0; y < h; y++)
        for (uint32_t x = 0; x < w; x++) {
          auto c = tmp.readPixelRGB(x, y);
          m.a8[y * w + x] = c.g;  // white icon on black: green = alpha
        }
      FUI_LOGD("Loaded icon mask: %s (%ux%u)", path, (unsigned)w, (unsigned)h);
    } else {
      FUI_LOGE("Failed to decode icon: %s", path);
    }
    tmp.deleteSprite();
    return ok ? &m : nullptr;
  }

  // Icon by name and size (16, 24, 32): <icon_dir>/<size>/<name>.png
  const IconMask* mask(const char* name, uint8_t size) {
    char path[96];
    snprintf(path, sizeof(path), "%s/%u/%s.png", _icon_dir.c_str(), (unsigned)size, name);
    return loadMask(path);
  }

  // Alpha-blend `color` over what is already on the canvas (works on gradients/images).
  // opacity scales the mask (255 = as is).
  static void drawMask(M5Canvas& c, const IconMask* m, int16_t x, int16_t y, uint16_t color, uint8_t opacity = 255) {
    if (!m || !m->a8) return;
    const uint16_t fr = color >> 11, fg = (color >> 5) & 0x3F, fb = color & 0x1F;
    for (uint8_t py = 0; py < m->h; py++) {
      for (uint8_t px = 0; px < m->w; px++) {
        uint16_t a = m->a8[py * m->w + px];
        if (opacity != 255) a = (a * opacity + 127) / 255;
        if (a == 0) continue;
        if (a == 255) { c.drawPixel(x + px, y + py, color); continue; }
        const uint16_t bg = c.readPixel(x + px, y + py);
        const uint16_t br = bg >> 11, bgg = (bg >> 5) & 0x3F, bb = bg & 0x1F;
        const uint16_t r = (fr * a + br * (255 - a)) / 255;
        const uint16_t g = (fg * a + bgg * (255 - a)) / 255;
        const uint16_t b = (fb * a + bb * (255 - a)) / 255;
        c.drawPixel(x + px, y + py, (uint16_t)((r << 11) | (g << 5) | b));
      }
    }
  }

  // Load (first use) + draw by name. false if the icon is missing.
  bool drawTinted(M5Canvas& c, const char* name, uint8_t size, int16_t x, int16_t y, uint16_t color, uint8_t opacity = 255) {
    const IconMask* m = mask(name, size);
    if (!m) return false;
    drawMask(c, m, x, y, color, opacity);
    return true;
  }

  // ---- Sprite icons (BMP, drawn as is) ----

  // Load icon from filesystem given to init() (BMP format)
  bool loadIcon(const char* path, M5Canvas& parent) {
    String key(path);

    // Check cache
    if (icon_cache.find(key) != icon_cache.end() && icon_cache[key].loaded) {
      return true;
    }

    if (!_fs) {
      FUI_LOGE("IconLoader not initialized (no fs): %s", path);
      return false;
    }
    File file = _fs->open(path, "r");
    if (!file) {
      FUI_LOGE("Failed to open icon: %s", path);
      return false;
    }

    // Get file size and read into buffer
    size_t filesize = file.size();
    uint8_t* buf = new uint8_t[filesize];
    file.read(buf, filesize);
    file.close();

    // BMP header: "BM", width at offset 18, height at 22 (negative = top-down)
    if (filesize < 26 || buf[0] != 'B' || buf[1] != 'M') {
      FUI_LOGE("Not a BMP file: %s", path);
      delete[] buf;
      return false;
    }
    int32_t w, h;
    memcpy(&w, buf + 18, sizeof(w));
    memcpy(&h, buf + 22, sizeof(h));
    if (h < 0) h = -h;

    // Create cache entry - the sprite must have a buffer before drawBmp() can draw into it
    Icon& icon = icon_cache[key];
    icon.init(&parent);
    icon.sprite.setPsram(true);  // cached for good: keep out of internal DRAM
    bool success = icon.sprite.createSprite(w, h) != nullptr;

    // Decode BMP from buffer into sprite
    if (success) success = icon.sprite.drawBmp(buf, filesize);
    delete[] buf;

    if (success) {
      icon.loaded = true;
      FUI_LOGD("Loaded icon: %s (%dx%d)", path, icon.sprite.width(), icon.sprite.height());
    } else {
      FUI_LOGE("Failed to decode icon: %s", path);
      icon.destroy();
    }

    return success;
  }

  // Draw cached icon at position
  bool drawIcon(M5Canvas& canvas, const char* path, int16_t x, int16_t y, bool invert = false, bool transparent = true, uint16_t transparent_color = TFT_MAGENTA) {
    String key(path);

    auto it = icon_cache.find(key);
    if (it == icon_cache.end() || !it->second.loaded) {
      FUI_LOGW("Icon not loaded: %s", path);
      return false;
    }

    M5Canvas& spr = it->second.sprite;
    if (!invert) {
      if (transparent) spr.pushSprite(&canvas, x, y, transparent_color);
      else             spr.pushSprite(&canvas, x, y);
      return true;
    }

    // Inverted: draw each pixel with inverted colour, skipping the transparent colour (icons are small)
    for (int16_t py = 0; py < spr.height(); py++) {
      for (int16_t px = 0; px < spr.width(); px++) {
        uint16_t c = spr.readPixel(px, py);
        if (transparent && c == transparent_color) continue;
        canvas.drawPixel(x + px, y + py, (uint16_t)~c);
      }
    }
    return true;
  }

  // Get icon dimensions (useful for layout calculations)
  bool getIconSize(const char* path, uint16_t& width, uint16_t& height) {
    String key(path);

    auto it = icon_cache.find(key);
    if (it == icon_cache.end() || !it->second.loaded) {
      return false;
    }

    width = it->second.sprite.width();
    height = it->second.sprite.height();
    return true;
  }

  // Clear all cached icons (free memory)
  void clearCache() {
    for (auto& pair : icon_cache) {
      pair.second.destroy();
    }
    icon_cache.clear();
    for (auto& pair : mask_cache) free(pair.second.a8);
    mask_cache.clear();
  }

  // Pre-load multiple icons (e.g., for wifi signal levels)
  bool loadIcons(M5Canvas& parent, const char* base_path, const char* prefix, int start, int end, const char* ext = ".bmp") {
    bool all_success = true;
    for (int i = start; i <= end; i++) {
      char path[64];
      snprintf(path, sizeof(path), "%s%s%d%s", base_path, prefix, i, ext);

      if (!loadIcon(path, parent)) {
        all_success = false;
      }
    }
    return all_success;
  }

  ~IconLoader() {
    clearCache();
  }
};

// Global singleton instance
extern IconLoader icon_loader;

} // namespace ui
