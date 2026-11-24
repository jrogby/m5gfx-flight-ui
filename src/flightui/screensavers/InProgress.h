#pragma once
#include "flightui/interfaces.h"
#include <stdlib.h>  // for rand() and srand()
#include <M5Unified.hpp>


struct InProgress : public IScreensaver {

  void setup() {
    initialized = false;
    _is_active = false;
    angle = 0.0f;
    zoom = 0.0f;
    last_draw = 0;
  }

  void start() override {
    _is_active = true;
    last_draw = millis();
  }

  void stop() override {
    initialized = false;
    _is_active = false;
  }

  bool is_active() const override { return _is_active; }
  bool redraw() const override { return true; }

  void update(ulong fast_update_millis) override {
    if (_is_active && (millis() - last_draw >= fast_update_millis)) {
      draw_screensaver();
      last_draw = millis();
    }
  }

 private:
  bool initialized;
  bool _is_active;
  float angle;
  float zoom;
  ulong last_draw;

  void draw_screensaver() {
    // start values
    static float initial_zoom = 1.0f;
    static float amplitude = 0.0f;
    static const float max_amplitude = 1.5f;
    static const float amplitude_inc = 0.11f;

    if (!initialized) {
        angle = 0.0f;
        amplitude = 0.0f;
        initialized = true;
    }

    angle += 0.042f;  // Increment angle for the sine wave

    // Gradually increase the amplitude until it reaches max_amplitude
    if (amplitude < max_amplitude) {
        amplitude += amplitude_inc;
    }

    float frequency = 20.0f;  // Frequency of the oscillation

    // Zoom starts at 1.0 and then oscillates around a base of 1.0 with increasing amplitude
    zoom = initial_zoom + amplitude * sin(angle / frequency);

    _buf->pushRotateZoom(_lcd, 160, 120, angle, zoom, zoom, 0);
  }
};