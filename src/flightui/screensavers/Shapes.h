#pragma once
#include "flightui/interfaces.h"
#include <stdlib.h>  // for rand() and srand()
#include <M5Unified.hpp>



static const u16 shapeColorPalette[] = {
    ui::color::SKYBLUE,
    ui::color::TX_SENT,
    ui::color::RX_PARSED,
    ui::color::ECAM_VALUE,
    ui::color::OFFWHITE,
    ui::color::BOEING_747_RED,
    ui::color::BOEING_747_GREEN,
    ui::color::BOEING_747_AMBER
};

struct Shapes : public IScreensaver {

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
  bool redraw() const override { return false; }

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
  float frequency; // Frequency of the sine wave
  ulong last_draw;
  uint16_t color_idx;

  void draw_screensaver() {
    // start values
    static float initial_zoom = 1.0f;
    static float amplitude = 0.0f;
    static const float max_amplitude = 1.8f;
    static const float amplitude_inc = 0.11f;
    static unsigned long last_random_pixel_time = 0;
    static unsigned long next_random_pixel_time = 0;

    // Initialize random seed once
    static bool is_seed_initialized = false;
    if (!is_seed_initialized) {
      srand(time(NULL));
      is_seed_initialized = true;
    }

    if (!initialized) {
        angle = (rand() % 100) / 100.0f;
        amplitude = (rand() % 200) / 200.0f;
        frequency =(rand() % 60) + 10;
        initialized = true;
    }
    ulong current_time = millis();
    // Time to generate random pixels?
    if (current_time - last_random_pixel_time >= next_random_pixel_time) {
        int len = 12;
        int x = rand() % len;
        int y = rand() % len;

        // Simulate depth
        float z = (rand() % 100) / 100.0f;  // z is between 0 (far) and 1 (near)

        // Apply perspective scaling based on depth (z)
        float scale = 1.0f + z * 3.0f;                      // Adjust scale factor as needed
        int scaled_x = (int)((x - (len/2)) * scale) + 160;  // Adjust for screen center
        int scaled_y = (int)((y - (len/2)) * scale) + 120;

        int pal_col = rand() % (sizeof(shapeColorPalette) / sizeof(shapeColorPalette[0]));

        // Draw the "3D" pixel
        _buf->drawPixel(scaled_x, scaled_y, shapeColorPalette[pal_col]);
        //disp_buf.drawPixel(scaled_x, scaled_y, color);
        //drawGradientHLine or something more fun?

        last_random_pixel_time = current_time;

        // Determine the next time interval (random between 3000ms to 9000ms)
        next_random_pixel_time = 50;  // (rand() % 6000) + 3000;
    }

    // if (angle >= 1.0f)
    //   angle += 0.042f;
    // if (angle <= 0.0f)
    //   angle -= 0.042f;
    angle += 0.042f;  // Increment angle for the sine wave

    // Gradually increase the amplitude until it reaches max_amplitude
    if (amplitude < max_amplitude) {
        amplitude += amplitude_inc;
    }



    // Zoom starts at 1.0 and then oscillates around a base of 1.0 with increasing amplitude
    zoom = initial_zoom + amplitude * sin(angle / frequency);

    color_idx = (color_idx + 1) % 6;

    // Psykadelic
    _buf->pushRotateZoom(_buf, 160, 120, angle, zoom + (angle * 0.01f), zoom + (angle * 0.1f), 0);
    _buf->pushSprite(0, 0); // After drawing to buffer to buffer, push to screen
  }
};