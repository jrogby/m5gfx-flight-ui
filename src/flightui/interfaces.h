#pragma once
#include <M5GFX.h>
#include "flightui/types.h"


/**
 * Touch event for the current page. Left/right swipes are page navigation and never reach the page.
 */
struct TouchEvent {
  enum Type : uint8_t { TAP, LONG_PRESS, SWIPE_UP, SWIPE_DOWN };
  Type type;
  int16_t x = 0, y = 0;  // touch point (TAP / LONG_PRESS), swipe start point
  uint16_t speed = 0;    // SWIPE_*: finger speed in px/s (distance / time from finger down to swipe); 0 otherwise
};


/**
 * Interface for ScreenPages
 */
struct IScreenPage {
  virtual ~IScreenPage() = default;

  virtual void setup() = 0;
  virtual void update() = 0;
  virtual void kill() = 0;

  bool has_title = true; // pages with title separator get page indicator
  uint16_t update_ms = 0; // redraw interval for animated pages, 0 = DisplayManager default (slow)

  // Touch on this page. Return true when handled: the page is redrawn at once (button feedback).
  virtual bool on_touch(const TouchEvent&) { return false; }
};


/**
 * Interface for Screensavers
 */
struct IScreensaver {
 public:
  virtual ~IScreensaver() = default;

  // Set by DisplayManager before start(): page canvas (source) and LCD (target)
  void attach(M5Canvas& buf, lgfx::LovyanGFX& lcd) { _buf = &buf; _lcd = &lcd; }

  virtual void start() = 0;
  virtual void update(ulong fast_update_millis) = 0; // to be called regularly
  virtual bool is_active() const = 0;
  virtual bool redraw() const = 0; // whether to redraw the display with normal slow update
  virtual void stop() = 0;

 protected:
  M5Canvas* _buf = nullptr;
  lgfx::LovyanGFX* _lcd = nullptr;
};