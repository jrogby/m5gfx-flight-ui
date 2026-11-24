#pragma once
#include <M5Unified.hpp>
#include "flightui/log.h"

/**
 * Gesture detection on raw press state + position (M5Unified's flick/click states are not used:
 * its 8 px flick threshold turned most finger taps into flicks, and its one-update click state was
 * lost when M5.update() and this class both called M5.Touch.update()).
 *
 * Touch state is updated only by M5.update() (app loop); update_state() just reads it.
 * - swipe: fires DURING the drag once the finger moved SWIPE_PX on the dominant axis
 * - tap:   on release, if the finger stayed within TAP_SLOP_PX and no swipe/hold fired
 * - hold:  while pressed, after HOLD_MS without moving more than TAP_SLOP_PX
 */
struct Touch {
  static constexpr int16_t TAP_SLOP_PX = 20;
  static constexpr int16_t SWIPE_PX = 30;
  static constexpr uint32_t HOLD_MS = 600;

  bool _left_swipe = false;
  bool _right_swipe = false;
  bool _up_swipe = false;
  bool _down_swipe = false;
  bool _long_click = false;
  bool _short_click = false;
  int16_t x = 0, y = 0;  // point of the last tap / hold / swipe start

  bool swiped_right() { return take(_right_swipe); }
  bool swiped_left()  { return take(_left_swipe); }
  bool swiped_up()    { return take(_up_swipe); }
  bool swiped_down()  { return take(_down_swipe); }
  bool long_pressed() { return take(_long_click); }
  bool short_pressed(){ return take(_short_click); }

  void update_state() {
    const auto& d = M5.Touch.getDetail();
    const bool pressed = M5.Touch.getCount() && d.isPressed();
    const uint32_t now = millis();

    if (pressed && !_down) {  // finger down
      _down = true;
      _used = false;
      _x0 = _xl = d.x; _y0 = _yl = d.y;
      _t0 = now;
      return;
    }
    if (pressed) {
      if (_used) return;  // one gesture per touch
      const int dx = d.x - _x0, dy = d.y - _y0;
      const int ax = abs(dx), ay = abs(dy);
      if (ax >= SWIPE_PX || ay >= SWIPE_PX) {
        x = _x0; y = _y0;
        if (ax >= ay) (dx > 0 ? _right_swipe : _left_swipe) = true;
        else          (dy > 0 ? _down_swipe : _up_swipe) = true;
        _used = true;
      } else if (now - _t0 >= HOLD_MS && ax <= TAP_SLOP_PX && ay <= TAP_SLOP_PX) {
        FUI_LOGD("Touch HOLD");
        x = _x0; y = _y0;
        _long_click = true;
        _used = true;
      }
      _xl = d.x; _yl = d.y;
      return;
    }
    if (_down) {  // finger up
      _down = false;
      if (!_used && abs(_xl - _x0) <= TAP_SLOP_PX && abs(_yl - _y0) <= TAP_SLOP_PX) {
        FUI_LOGD("Touch TAP");
        x = _x0; y = _y0;
        _short_click = true;
      }
    }
  }

  bool is_down() const { return _down; }

 private:
  bool _down = false, _used = false;
  int16_t _x0 = 0, _y0 = 0, _xl = 0, _yl = 0;
  uint32_t _t0 = 0;

  static bool take(bool& f) { const bool v = f; f = false; return v; }
};
