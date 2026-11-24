// Draw text with an embedded VLW font - no filesystem, no uploadfs.
#include <M5Unified.h>
#include <FlightUIFonts.h>
#include <flightui/fonts/slots.h>  // ECAM slots: fallback font unless a font pack (e.g. m5gfx-fonts-ecam) is installed

void setup() {
  M5.begin();
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.loadFont(flightui::font::ECAMFontRegular22);
  M5.Display.setTextColor(TFT_GREEN);
  M5.Display.drawString("ECAM slot 22", 10, 10);
  M5.Display.loadFont(flightui::font::HoneywellMCDU22);
  M5.Display.drawString("MCDU 22", 10, 50);
}

void loop() {}
