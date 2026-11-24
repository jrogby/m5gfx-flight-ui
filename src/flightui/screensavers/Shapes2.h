  /************/

//   void draw_screensaver() {
//     static unsigned long last_pixel_time = 0;
//     static unsigned long next_pixel_time = 0;

//     // Initialize random seed once
//     static bool is_seed_initialized = false;
//     if (!is_seed_initialized) {
//       srand(time(NULL));
//       is_seed_initialized = true;
//     }

//     ulong current_time = millis();  // Assuming you have a millis() function for current time in ms

//     // Determine if it's time to generate random pixels
//     if (current_time - last_pixel_time >= next_pixel_time) {
//         // Generate random x and y positions
//         int len = 12;
//         int x = rand() % len;
//         int y = rand() % len;

//         // Simulate a z-coordinate (depth)
//         float z = (rand() % 100) / 100.0f;  // z is between 0 (far) and 1 (near)

//         // Apply perspective scaling based on depth (z)
//         float scale = 1.0f + z * 3.0f;  // Adjust scale factor as needed
//         int scaled_x = (int)((x - (len/2)) * scale) + 160;  // Adjust for screen center
//         int scaled_y = (int)((y - (len/2)) * scale) + 120;

// //Draw a pixel for psykadelic
//         // Generate color based on depth (darker for far away)
//         //uint16_t color = (uint16_t)(pal_col * (0.5f + z * 0.5f));  // Mix base color with depth factor
//         // Draw the "3D" pixel
//         //disp_buf.drawPixel(scaled_x, scaled_y, color);
//         // int pal_col = rand() % sizeof(screensaverPalette);
//         //disp_buf.drawPixel(scaled_x, scaled_y, screensaverPalette[pal_col]);
//         // drawGradientHLine

//         // Update last pixel time
//         //last_pixel_time = current_time;

//         // Determine the next time interval (random between 3000ms to 9000ms)
//         //next_pixel_time = 50;  // (rand() % 6000) + 3000;
//     }


//     // Nice!!!
//     // screensaver_angle += 0.1f;
//     // screensaver_zoom = 1.0f + 0.2f*sin(screensaver_angle/10.0f);  // sine wave zoom
//     // screensaver_color_idx = (screensaver_color_idx + 1) % 6;
//     // disp_buf.pushRotateZoom(&disp_buf, 160, 120, screensaver_angle, screensaver_zoom, screensaver_zoom, screensaverPalette[screensaver_color_idx]);
//     if (screensaver_angle >= 1.0f)
//       screensaver_angle += 0.046f;
//     if (screensaver_angle <= 0.0f)
//       screensaver_angle -= 0.046f;

//     float base_zoom = 1.6f; // base zoom level. The zoom will oscillate around this value.
//     float amplitude = 0.6f; // amplitude of the oscillation. controls how much the zoom level fluctuates above and below the base value.
//     float frequency = 20.0f; // frequency of the oscillation. controls how fast the sine wave cycles.
//     screensaver_zoom = base_zoom + amplitude * sin(screensaver_angle / frequency);  // sine wave zoom
//     screensaver_color_idx = (screensaver_color_idx + 1) % 6;

//     // Psykadelic
//     //disp_buf.pushRotateZoom(&disp_buf, 160, 120, screensaver_angle, screensaver_zoom + (screensaver_angle * 0.01f), screensaver_zoom + (screensaver_angle * 0.1f), 0);
//     //disp_buf.pushSprite(0, 0);

//     // Melting
//     disp_buf.pushRotateZoom(&disp, 160, 120, screensaver_angle, screensaver_zoom, screensaver_zoom, 0);
//   }
/*******/