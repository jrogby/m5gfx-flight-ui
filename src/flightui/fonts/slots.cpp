#include "flightui/fonts/slots.h"
#include "FlightUIFonts.h"
#include "flightui/log.h"

namespace flightui::font {

// Fallback VLW: nearest Honeywell MCDU size (OFL, bundled)
static constexpr const uint8_t* vlw_fallback(int px) {
  return px <= 10 ? HoneywellMCDU10 : px <= 12 ? HoneywellMCDU12 : px <= 14 ? HoneywellMCDU14
       : px <= 16 ? HoneywellMCDU16 : px <= 18 ? HoneywellMCDU18 : px <= 20 ? HoneywellMCDU20
       : HoneywellMCDU22;
}

// Fallback GFX: nearest built-in FreeSans (M5GFX)
static const lgfx::IFont* gfx_fallback(int pt, bool bold) {
  if (pt <= 10) return bold ? (const lgfx::IFont*)&fonts::FreeSansBold9pt7b  : &fonts::FreeSans9pt7b;
  if (pt <= 15) return bold ? (const lgfx::IFont*)&fonts::FreeSansBold12pt7b : &fonts::FreeSans12pt7b;
  return bold ? (const lgfx::IFont*)&fonts::FreeSansBold18pt7b : &fonts::FreeSans18pt7b;
}

#define FUI_DEF_VLW(n)  const uint8_t* ECAMFontRegular##n = vlw_fallback(n);
#define FUI_DEF_GFXR(n) const lgfx::IFont* ECAMFontRegular##n##pt7b = gfx_fallback(n, false);
#define FUI_DEF_GFXB(n) const lgfx::IFont* ECAMFontBold100em##n##pt7b = gfx_fallback(n, true);
FLIGHTUI_ECAM_VLW_SLOTS(FUI_DEF_VLW)
FLIGHTUI_ECAM_GFX_REGULAR_SLOTS(FUI_DEF_GFXR)
FLIGHTUI_ECAM_GFX_BOLD_SLOTS(FUI_DEF_GFXB)

int warn_fallback_slots() {
  int n = 0;
#define FUI_CHK(name, fb) if (name == (fb)) { FUI_LOGW("font slot %s uses stand-in font", #name); n++; }
#define FUI_CHK_VLW(n_)  FUI_CHK(ECAMFontRegular##n_, vlw_fallback(n_))
#define FUI_CHK_GFXR(n_) FUI_CHK(ECAMFontRegular##n_##pt7b, gfx_fallback(n_, false))
#define FUI_CHK_GFXB(n_) FUI_CHK(ECAMFontBold100em##n_##pt7b, gfx_fallback(n_, true))
  FLIGHTUI_ECAM_VLW_SLOTS(FUI_CHK_VLW)
  FLIGHTUI_ECAM_GFX_REGULAR_SLOTS(FUI_CHK_GFXR)
  FLIGHTUI_ECAM_GFX_BOLD_SLOTS(FUI_CHK_GFXB)
#undef FUI_CHK_VLW
#undef FUI_CHK_GFXR
#undef FUI_CHK_GFXB
#undef FUI_CHK
  return n;
}

}  // namespace flightui::font
