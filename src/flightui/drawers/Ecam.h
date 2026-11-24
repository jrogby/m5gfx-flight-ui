#pragma once
#include <M5Unified.hpp>
#include "flightui/types.h"
#include "flightui/color.h"
#include "flightui/fonts/vlw.h"
#include "flightui/fonts/gfx.h"


// Airbus ECAM v1
struct Ecam {

  /**
   * Create a 320x30 page title sprite
   *
   * - Using a smaller font size than the "original", but a good size for the small M5 screen.
   *
   * NOTE: Font only support capitals!
   */
  // void createPageTitle(M5Canvas &c, String title) {
  //   auto x = 320;
  //   auto x2 = x/2;
  //   auto y = 24;
  //   // auto tl = title.length() * 7;
  //   c.createSprite(320,y);
  //   c.setTextDatum(TC_DATUM);
  //   c.setTextColor(ui::color::ECAM_TITLE);

  //   // // c.loadFont(LittleFS, "/font/ECAMFontRegular26.vlw");
  //   c.loadFont(ECAMFontRegular18);
  //   // // c.drawString(title, x2, 2, ECAMFontRegular15pt7b);
  //   c.drawString(title, x2, 2);
  //   // c.drawLine(x2-tl-2, y-1, x2+tl, y-1, ui::color::ECAM_TITLE);

  //   c.drawLine(10, y-1, 310, y-1, ui::color::ECAM_ELEC_BG);
  // }
void createPageTitle(M5Canvas &c, const String& title) {
    const uint16_t width = 320;
    const uint16_t height = 24;
    const uint16_t x_center = width / 2;

    c.setPsram(true); // Enable PSRAM for the canvas
    c.createSprite(width, height);
    c.setTextDatum(TC_DATUM);
    c.setTextColor(ui::color::ECAM_TITLE);

    c.loadFont(ECAMFontRegular18);
    String title_caps = title;
    title_caps.toUpperCase();
    c.drawString(title_caps, x_center, 2);

    // Draw the line below the title
    c.drawLine(10, height - 1, width - 10, height - 1, ui::color::ECAM_ELEC_BG);
}

void showAirbusPageTitle(M5Canvas &buf, String title) {
    buf.setTextDatum(TC_DATUM);
    buf.setTextColor(ui::color::ECAM_TITLE);
    buf.drawString(title,160,2,ECAMFontRegular15pt7b); // Smaller than original, but good size for screen
    uint tw = (title.length()*17) + (title.length()*2); // 18*5
    uint x_start = 159-tw/2;
    uint x_end =   160+tw/2;
    buf.drawLine(x_start, 28, x_end, 28, ui::color::ECAM_TITLE);
    buf.setTextDatum(0);
  }

  /**
   * Creates a 320x43 stats-row sprite.
   * Label max 5 chars, unit max 3 chars, value max 6 chars
   */
  // void createStatsRow(M5Canvas &c, uint row_num, String label = "", String unit = "", String init_val = "") {
  //   // uint fw = 24;
  //   // uint vx;
  //   // if(label.length() <= 1) {vx = fw*2;}
  //   // if(label.length() == 2) {vx = fw*3;}
  //   // if(label.length() == 3) {vx = fw*4;}
  //   // if(label.length() == 4) {vx = fw*5;}
  //   // if(label.length() == 5) {vx = fw*6;}
  //   // if(label.length() == 6) {vx = fw*7;}
  //   // if(label.length() == 7) {vx = fw*8;}
  //   // if(label.length() >= 8) {vx = fw*9;}

  //   uint rh = 32;
  //   c.createSprite(320,rh);
  //   c.setTextDatum(TL_DATUM);
  //   // c.loadFont(LittleFS, "/font/ECAMFontRegular28.vlw");

  //   c.loadFont(ECAMFontRegular24);
  //   c.setTextColor(ui::color::ECAM_LABEL); c.drawString(label,      4,0); // Bold 19pt white for labels, same size as value
  //   //c.setTextColor(ui::color::ECAM_VALUE); c.drawString(init_val,  vx,0); // Bold 19pt green for values, after decimal same size
  //   c.loadFont(ECAMFontRegular18);
  //   c.setTextColor(ui::color::ECAM_UNIT);  c.drawString(unit,     273,3); // Bold 16pt cyan for units
  //   // c.setTextColor(ui::color::ECAM_LABEL); c.drawString(label,      0,0,ECAMFontBold100em19pt7b); // Bold 19pt white for labels, same size as value
  //   // c.setTextColor(ui::color::ECAM_VALUE); c.drawString(init_val,  vx,0,ECAMFontBold100em19pt7b); // Bold 19pt green for values, after decimal same size
  //   // c.setTextColor(ui::color::ECAM_UNIT);  c.drawString(unit,     260,5,ECAMFontBold100em15pt7b); // Bold 16pt cyan for units
  //   c.pushSprite(0,(row_num*rh));
  // }
void createStatsRow(M5Canvas &c, uint row_num, const String& label = "", const String& unit = "", const String& init_val = "") {
    const uint16_t row_height = 32;
    const uint16_t width = 320;

    c.setPsram(true); // Enable PSRAM for the canvas
    c.createSprite(width, row_height);
    c.setTextDatum(TL_DATUM);

    // Draw label
    c.loadFont(ECAMFontRegular24);
    c.setTextColor(ui::color::ECAM_LABEL);
    c.drawString(label, 4, 0);

    // Draw unit
    c.loadFont(ECAMFontRegular18);
    c.setTextColor(ui::color::ECAM_UNIT);
    c.drawString(unit, width - 36, 3);

    // Push sprite to the appropriate row position
    c.pushSprite(0, row_num * row_height);
}
  // void updateStatsRowVal(M5Canvas &c, uint row_num, float new_val)
  // {
  //   char buffer[10];
  //   sprintf(buffer, "%2.2f", new_val);
  //   String vs = buffer;
  //   updateStatsRowVal(c, row_num, vs);
  // }

  // void updateStatsRowVal(M5Canvas &c, uint row_num, double new_val)
  // {
  //   char buffer[10];
  //   sprintf(buffer, "%2.2f", new_val);
  //   String vs = buffer;
  //   updateStatsRowVal(c, row_num, vs);
  // }

  // void updateStatsRowVal(M5Canvas &c, uint row_num, String new_val, String left_padding = " ")
  // {
  //   c.setTextDatum(TR_DATUM);

  //   if(new_val.equals("0.00") || new_val.equals("0.0") || new_val.equals("0"))
  //     c.setTextColor(ui::color::BOEING_AMBER, ui::color::ECAM_BG);
  //   else
  //     c.setTextColor(ui::color::ECAM_VALUE,   ui::color::ECAM_BG);

  //   c.loadFont(ECAMFontRegular24);

  //   c.drawString(left_padding + new_val,288,0);
  //   uint row_height = 32;
  //   c.pushSprite(0,(row_num*row_height));
  // }

  void updateStatsRowVal(M5Canvas &c, uint row_num, float new_val) {
    updateStatsRowVal(c, row_num, String(new_val, 2));
}

void updateStatsRowVal(M5Canvas &c, uint row_num, double new_val) {
    updateStatsRowVal(c, row_num, String(new_val, 2));
}
  void updateStatsRowVal(M5Canvas &c, uint row_num, const String& new_val, const String& left_padding = "  ") {
    const uint16_t row_height = 32;
    const uint16_t width = 320;
    const uint16_t value_pos_x = width - 38;

    c.setTextDatum(TR_DATUM);

    // Set color based on value
    if (new_val == "0.00" || new_val == "0.0" || new_val == "0") {
        c.setTextColor(ui::color::BOEING_AMBER, ui::color::ECAM_BG);
    } else {
        c.setTextColor(ui::color::ECAM_VALUE, ui::color::ECAM_BG);
    }

    c.loadFont(ECAMFontRegular24);
    const uint16_t clear_x = 120; // after label area
    c.fillRect(clear_x, 0, value_pos_x - clear_x, row_height, ui::color::ECAM_BG);
    c.drawString(left_padding + new_val, value_pos_x, 0);

    // Push updated sprite to the appropriate row position
    c.pushSprite(0, row_num * row_height);
}


  void showAirbusElecDC(M5Canvas &buf, uchar x, uchar y) {
    buf.setTextDatum(TC_DATUM);
    buf.setTextColor(ui::color::ECAM_ELEC_LABEL, ui::color::ECAM_ELEC_BG);
    buf.drawString(" DC BAT ",x,y,ECAMFontBold100em15pt7b); // Smaller than original, but good size for screen
    buf.setTextDatum(0);
  }


  void showAirbusLowerStats(M5Canvas &buf, int x, int y) {
    /*** In bottom 3xbox status view ***/
    buf.setTextDatum(BL_DATUM);
    buf.setTextColor(ui::color::ECAM_LABEL); // White for labels
    buf.drawString("GW           ",x,y,ECAMFontBold100em19pt7b); // Bold for labels, same size as value
    buf.setTextColor(ui::color::ECAM_VALUE); // Green for values
    buf.drawString("    147980   ",x,y,ECAMFontBold100em19pt7b); // Bold for values, after decimal same size
    buf.setTextColor(ui::color::ECAM_UNIT); // Cyan for trailing units
    buf.drawString("              KG",x,y,ECAMFontBold100em16pt7b); // Bold smaller font for trailing units
    // Row distance? =
    buf.setTextColor(ui::color::ECAM_LABEL); // White for labels
    buf.drawString("GWCG         ",x,y+43,ECAMFontBold100em19pt7b); // Bold for labels, same size as value
    buf.setTextColor(ui::color::ECAM_VALUE); // Green for values
    buf.drawString("      38.6   ",x,y+43,ECAMFontBold100em19pt7b); // Bold for values, after decimal same size
    buf.setTextColor(ui::color::ECAM_UNIT); // Cyan for trailing units
    buf.drawString("               %",x,y+43,ECAMFontBold100em16pt7b); // Bold smaller font for trailing units
    buf.setTextDatum(0);
  }


  // UTC from the system clock (app keeps it synced: RTC at boot, then NTP/N2K), no RTC I2C read per frame
  void showAirbusLowerClock(M5Canvas &buf, int x, int y) {
    time_t now = time(nullptr);
    tm utc;
    gmtime_r(&now, &utc);
    String hours(utc.tm_hour);
    if(hours.length() < 2)
      hours = "0" + hours + "     ";
    else
      hours = hours + "    ";

    String minutes(utc.tm_min);
    if(minutes.length() < 2) minutes = "     0" + minutes;
    else minutes = "     " + minutes;
    buf.setTextDatum(BC_DATUM);
    buf.setTextColor(ui::color::ECAM_VALUE); // Clock hour
    buf.drawString(hours,x,y,ECAMFontBold100em19pt7b);
    buf.setTextColor(ui::color::ECAM_UNIT); // Cyan for hour sign
    buf.drawString("   H   ",x,y,ECAMFontBold100em16pt7b);
    buf.setTextColor(ui::color::ECAM_VALUE); // Clock minute
    buf.drawString(minutes,x,y,ECAMFontBold100em19pt7b);
    buf.setTextDatum(0);
  }

  void showAirbusLowerClockSmall(M5Canvas &buf, int x, int y) {
    buf.setTextDatum(BC_DATUM);
    buf.setTextColor(ui::color::ECAM_VALUE); // Clock hour
    buf.drawString("17     ",x,y,ECAMFontBold100em14pt7b);
    buf.setTextColor(ui::color::ECAM_UNIT); // Cyan for hour sign
    buf.drawString("   H   ",x,y,ECAMFontBold100em13pt7b);
    buf.setTextColor(ui::color::ECAM_VALUE); // Clock minute
    buf.drawString("     12",x,y,ECAMFontBold100em14pt7b);
    buf.setTextDatum(0);
  }

};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ECAM)
inline Ecam ECAM;
#endif

// FROM setup()
//   ECAM.createPageTitle(page_title, "CRUISE"); page_title.pushSprite(0,0);
//   ECAM.createStatsRow(stats_row_a,1,"T1* ",    "  C","-");
//   ECAM.createStatsRow(stats_row_b,2,"PRES ",   "MBA","-");
//   ECAM.createStatsRow(stats_row_c,3,"HUMID  ","  %","-");
//   ECAM.createStatsRow(stats_row_d,4,"STW", " KT", "0000.0");
//   ECAM.createStatsRow(stats_row_e,5,"GW",  " KG", "0000.0");
// FROM loop()
//   char buffer[5];
//   sprintf(buffer, "Â°C");
//   ECAM.showAirbusPageTitle(disp_buf, String(buffer));
//   ECAM.showAirbusPageTitle(disp_buf, "*-@+%");
//   ECAM.showAirbusElecDC(disp_buf, 160, 80);

//   ECAM.showAirbusLowerStats(0, 80);
//   drawLayoutVerticalLines(disp_buf, 24, 11, 220, 0, TFT_GOLD); // label + value
//   drawLayoutHorizontalLines(disp_buf, 38, 16, 320, 1, 29, TFT_BROWN); // label + value
//   drawLayoutHorizontalLines(disp_buf, 38, 14, 320, 0, 0, TFT_MAGENTA); // label + value

//   ECAM.showAirbusPageTitle(disp_buf, "SENSORS");
//   ECAM.updateStatsRowVal(stats_row_a, 1, "T1* ", String(m5sd.temp_degC));
//   ECAM.updateStatsRowVal(stats_row_b, 2, "PRES ", String((int)m5sd.pressure_mBar));
//   ECAM.updateStatsRowVal(stats_row_c, 3, "HUMID  ", String((int)m5sd.humidity));

//   disp_buf.drawLine(0, 180, 320, 180, TFT_WHITE); // lower horizontal line, big clock/lower stats
//   ECAM.showAirbusLowerClock(disp_buf,160,220);
//   ECAM.showAirbusLowerClockSmall(disp_buf,160,220);


// Airbus ECAM v2
struct Ecam2 {

  lgfx::v1::TextStyle title_text_style;

  void init() {
    // create text styles
    M5Canvas cv;
    cv.setPsram(true); // Enable PSRAM for the canvas
    cv.createSprite(32,32);
    cv.setTextDatum(textdatum_t::baseline_center);
    cv.setTextPadding(1);

    title_text_style = cv.getTextStyle();

    cv.loadFont("");
  //     float size_x = 1;
  //     float size_y = 1;
  //     textdatum_t datum = textdatum_t::top_left;
  //     int32_t padding_x = 0;
  //     bool utf8 = true;
  //     bool cp437 = false;
  //   struct TextStyle
  //   {
  //     uint32_t fore_rgb888 = 0xFFFFFFU;
  //     uint32_t back_rgb888 = 0;
  //     float size_x = 1;
  //     float size_y = 1;
  //     textdatum_t datum = textdatum_t::top_left;
  //     int32_t padding_x = 0;
  //     bool utf8 = true;
  //     bool cp437 = false;
  //     // IFont* font = &fonts::Font0;
  //     // FontMetrics metrics;
  //  };
  }


  /**
   * Create a 320x30 page title sprite
   *
   * - Using a smaller font size than the "original", but a good size for the small M5 screen.
   *
   * NOTE: Font only support capitals!
   */
  void createPageTitle(M5Canvas &c, String title) {
    auto x = 320;
    auto x2 = x/2;
    auto y = 30;
    auto tl = title.length() * 9;
    c.setPsram(true); // Enable PSRAM for the canvas
    c.createSprite(320,y);
    c.setTextDatum(TC_DATUM);
    c.setTextColor(ui::color::ECAM_TITLE);
    c.drawString(title, x2, 2, ECAMFontRegular15pt7b);
    c.drawLine(x2-tl-2, y-1, x2+tl, y-1, ui::color::ECAM_TITLE);

  }

  void showAirbusPageTitle(M5Canvas &buf, String title) {
    buf.setTextDatum(TC_DATUM);
    buf.setTextColor(ui::color::ECAM_TITLE);
    buf.drawString(title,160,2,ECAMFontRegular15pt7b); // Smaller than original, but good size for screen
    uint tw = (title.length()*17) + (title.length()*2); // 18*5
    uint x_start = 159-tw/2;
    uint x_end =   160+tw/2;
    buf.drawLine(x_start, 28, x_end, 28, ui::color::ECAM_TITLE);
    buf.setTextDatum(0);
  }

  /**
   * Creates a 320x43 stats-row sprite.
   * Label max 5 chars, unit max 3 chars, value max 6 chars
   */
  void createStatsRow(M5Canvas &c, uint row_num, String label, String unit, String init_val = "") {
    uint fw = 23;
    uint vx;
    if(label.length() <= 1) {vx = fw*2;}
    if(label.length() == 2) {vx = fw*3;}
    if(label.length() == 3) {vx = fw*4;}
    if(label.length() == 4) {vx = fw*5;}
    if(label.length() == 5) {vx = fw*6;}
    if(label.length() == 6) {vx = fw*7;}
    if(label.length() == 7) {vx = fw*8;}
    if(label.length() >= 8) {vx = fw*9;}

    uint rh = 38;
    c.setPsram(true); // Enable PSRAM for the canvas
    c.createSprite(320,rh);
    c.setTextDatum(TL_DATUM);
    c.setTextColor(ui::color::ECAM_LABEL); c.drawString(label,      0,0,ECAMFontBold100em19pt7b); // Bold 19pt white for labels, same size as value
    c.setTextColor(ui::color::ECAM_VALUE); c.drawString(init_val,  vx,0,ECAMFontBold100em19pt7b); // Bold 19pt green for values, after decimal same size
    c.setTextColor(ui::color::ECAM_UNIT);  c.drawString(unit,     300,5,ECAMFontBold100em15pt7b); // Bold 16pt cyan for units
    c.pushSprite(0,(row_num*rh));
  }

  void updateStatsRowVal(M5Canvas &c, uint row_num, u8 label_len, float new_val) {
    char buffer[10];
    sprintf(buffer, "%2.2f", new_val);
    String vs = buffer;
    updateStatsRowVal(c, row_num, label_len, vs);
  }

  void updateStatsRowVal(M5Canvas &c, uint row_num, u8 label_len, double new_val) {
    char buffer[10];
    sprintf(buffer, "%2.2f", new_val);
    String vs = buffer;
    updateStatsRowVal(c, row_num, label_len, vs);
  }

  void updateStatsRowVal(M5Canvas &c, uint row_num, u8 label_len, String new_val) {
    uint fw = 23;
    uint vx;
    if(label_len <= 1) {vx = fw*2;}
    if(label_len == 2) {vx = fw*3;}
    if(label_len == 3) {vx = fw*4;}
    if(label_len == 4) {vx = fw*5;}
    if(label_len == 5) {vx = fw*6;}
    if(label_len == 6) {vx = fw*7;}
    if(label_len == 7) {vx = fw*8;}
    if(label_len >= 8) {vx = fw*9;}

    c.setTextDatum(TL_DATUM); // Already set in createStatsRow
    c.setTextColor(ui::color::ECAM_VALUE,ui::color::ECAM_BG);
    c.drawString(new_val, vx,0,ECAMFontBold100em19pt7b); // Bold 19pt green for values, after decimal same size
    uint row_height = 38;
    c.pushSprite(0,(row_num*row_height));
  }

  void showAirbusElecDC(M5Canvas &buf, uchar x, uchar y) {
    buf.setTextDatum(TC_DATUM);
    buf.setTextColor(ui::color::ECAM_ELEC_LABEL, ui::color::ECAM_ELEC_BG);
    buf.drawString(" DC BAT ",x,y,ECAMFontBold100em15pt7b); // Smaller than original, but good size for screen
    buf.setTextDatum(0);
  }

};
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ECAM2)
inline Ecam2 ECAM2;
#endif
