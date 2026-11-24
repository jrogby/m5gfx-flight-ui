#pragma once
#include <M5GFX.h>
#include "flightui/color.h"
#include "flightui/fonts/vlw.h"

namespace ui {

/**
 * ECAM style touch button (MCDU key look): rounded frame, label centered.
 * Draw every frame with draw(); feed taps with hit()/press() from IScreenPage::on_touch().
 *
 *   TouchButton btn{10, 190, 90, 36, "RESET"};
 *   bool on_touch(const TouchEvent& e) override {
 *     if(e.type == TouchEvent::TAP && btn.hit(e.x, e.y)) { btn.press(); reset(); return true; }
 *     return false;
 *   }
 *
 * press() flashes the button for FLASH_MS. toggle = true keeps an on/off state (green label when on).
 * Hit area grows by PAD px each side: fingers are bigger than the 2 px frame.
 */
struct TouchButton {
  static constexpr uint32_t FLASH_MS = 250;
  static constexpr int16_t PAD = 4;

  int16_t x, y, w, h;
  const char* label;
  bool toggle = false;  // keeps state: on/off
  bool on = false;
  bool enabled = true;
  uint32_t flash_until = 0;

  bool hit(int16_t px, int16_t py) const {
    return enabled && px >= x - PAD && px < x + w + PAD && py >= y - PAD && py < y + h + PAD;
  }

  void press() {
    flash_until = millis() + FLASH_MS;
    if(toggle) on = !on;
  }

  bool flashing() const { return (int32_t)(flash_until - millis()) > 0; }

  void draw(lgfx::LovyanGFX& c, const uint8_t* font = ECAMFontRegular14) const {
    using namespace ui::color;
    const bool fl = flashing();
    const uint16_t frame = !enabled ? ECAM_ELEC_BG : fl ? AIRBUS_YELLOW : AIRBUS_BLUE;
    const uint16_t fill = fl ? color565(40, 60, 80) : TFT_BLACK;
    const uint16_t text = !enabled ? ECAM_ELEC_BG : (toggle && on) ? AIRBUS_GREEN : fl ? AIRBUS_YELLOW : ECAM_LABEL;
    c.fillRoundRect(x, y, w, h, 4, fill);
    c.drawRoundRect(x, y, w, h, 4, frame);
    c.drawRoundRect(x + 1, y + 1, w - 2, h - 2, 3, frame);
    c.loadFont(font);
    c.setTextDatum(MC_DATUM);
    c.setTextColor(text);
    c.drawString(label, x + w / 2, y + h / 2 + 1);
    c.setTextDatum(TL_DATUM);
  }
};

}  // namespace ui
