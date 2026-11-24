# m5gfx-flight-ui

[![build examples](https://github.com/jrogby/m5gfx-flight-ui/actions/workflows/ci.yml/badge.svg?event=status)](https://github.com/jrogby/m5gfx-flight-ui/actions/workflows/ci.yml)

Aviation-style (Airbus ECAM / MCDU) GUI framework for M5Stack Core2 / Tough, built on M5GFX.

- **DisplayManager**: page list, swipe navigation, touch gestures (tap, long press, swipe), redraw timing, screensaver.
- **Pages**: implement `IScreenPage` (`setup` / `update` / `kill`, optional `on_touch`).
- **Drawers**: ECAM titles, table rows, annunciation board, touch buttons, modal screens.
- **Fonts**: VLW fonts compiled into flash, so no filesystem and no `uploadfs` are needed.
- No knowledge of your data model: your pages read your data and draw.

Status: **0.1.0**, API not stable yet (breaking changes possible before 1.0).

## Install (PlatformIO)

```ini
[env:core2]
platform = espressif32
framework = arduino
board = m5stack-core2
lib_deps = https://github.com/jrogby/m5gfx-flight-ui.git#v0.1.0
build_unflags = -std=gnu++11
build_flags = -std=gnu++17
```

Requires C++17.

## Quick start

```cpp
#include <M5Unified.h>
#include <ReactESP.h>
#include <flightui/DisplayManager.h>

reactesp::EventLoop events;
M5Canvas canvas(&M5.Display);

struct HelloPage : IScreenPage {
  void setup() override {}
  void update() override {
    canvas.fillScreen(TFT_BLACK);
    canvas.loadFont(flightui::font::ECAMFontRegular22);
    canvas.setTextColor(ui::color::AIRBUS_GREEN);
    canvas.drawString("HELLO", 20, 100);
  }
  void kill() override {}
} hello;

void setup() {
  M5.begin();
  canvas.setPsram(true);
  canvas.createSprite(M5.Display.width(), M5.Display.height());
  display_manager.add_page(&hello);
  display_manager.begin(events, canvas, M5.Display);
  events.onRepeat(10, [] { M5.update(); });   // DisplayManager reads touch state set by M5.update()
}

void loop() { events.tick(); }
```

Full examples: [examples/pages](examples/pages/pages.ino) (two pages + touch button),
[examples/embedded_fonts](examples/embedded_fonts/embedded_fonts.ino).

Left / right swipes change page and never reach the page. Taps, long presses and up / down swipes go to
`IScreenPage::on_touch()`; return `true` to redraw at once.

## Fonts

VLW smooth fonts are compiled into flash as `const uint8_t[]`:

- No filesystem, no `uploadfs`, no LittleFS / SPIFFS choice for the user.
- Zero RAM: M5GFX reads glyphs straight from memory-mapped flash.
- One `.cpp` per font: the linker drops sizes you never reference.

```cpp
#include <FlightUIFonts.h>
canvas.loadFont(flightui::font::HoneywellMCDU16);
```

### ECAM font slots

The ECAM font is GPL, so it is **not** in this library. Drawers use named slots from
`flightui/fonts/slots.h` (`ECAMFontRegular18`, `ECAMFontBold100em19pt7b`, ...). Unfilled slots fall back to
Honeywell MCDU (VLW) / FreeSans (GFX), and a warning is logged at `begin()`. A font pack can fill them at boot.

### Adding / changing a font

1. Put the `.vlw` in `assets/fonts/` (create it with Processing `Create Font` or TFT_eSPI's tool).
2. `python3 tools/vlw2h.py`
3. Commit both the asset and the generated `src/` files.

## Icons

`ui::icon_loader` draws tinted PNG icons (white on transparent) from a filesystem you mount yourself:
`ui::icon_loader.init(LittleFS)`. The library never mounts or unmounts a filesystem.

## Logging

Library logs go through `M5.Log` with tag `[ui]`. Set `ui::log_level` (default `ESP_LOG_INFO`).

## Layout

```
assets/fonts/   source .vlw files + font licenses (not compiled)
src/            library sources (FlightUIFonts.h, flightui/*, generated src/fonts/*.cpp)
tools/          font generator
docs/           notes
examples/       Arduino / pio ci examples
```

## License

Code: [MIT](LICENSE). Bundled Honeywell MCDU font: SIL OFL 1.1, © 2021 Tyler Knox,
see [assets/fonts/FONT_LICENSES.md](assets/fonts/FONT_LICENSES.md) and [OFL.txt](assets/fonts/OFL.txt).

See [ROADMAP.md](ROADMAP.md) for scope and milestones, [CHANGELOG.md](CHANGELOG.md) for changes.
