#pragma once
#include <M5GFX.h>

/**
 * Font slots. Drawers/pages use these names; a font pack fills them at boot
 * (e.g. ecam_fonts::install() from the separate, GPL m5gfx-fonts-ecam lib).
 * Unfilled slots fall back to fonts shipped with this lib (Honeywell MCDU, OFL)
 * or built-in M5GFX FreeSans, so the lib works stand-alone.
 *
 * VLW slots:  canvas.loadFont(ECAMFontRegular18);
 * GFX slots:  canvas.drawString(s, x, y, ECAMFontBold100em19pt7b);
 */
#define FLIGHTUI_ECAM_VLW_SLOTS(X) \
  X(8) X(9) X(10) X(11) X(12) X(13) X(14) X(15) X(16) X(17) X(18) X(19) X(20) \
  X(22) X(24) X(26) X(28) X(31) X(32) X(34)
#define FLIGHTUI_ECAM_GFX_REGULAR_SLOTS(X) X(10) X(13) X(15) X(17)
#define FLIGHTUI_ECAM_GFX_BOLD_SLOTS(X) X(7) X(8) X(10) X(12) X(13) X(14) X(15) X(16) X(19)

namespace flightui::font {
#define FUI_DECL_VLW(n)  extern const uint8_t* ECAMFontRegular##n;
#define FUI_DECL_GFXR(n) extern const lgfx::IFont* ECAMFontRegular##n##pt7b;
#define FUI_DECL_GFXB(n) extern const lgfx::IFont* ECAMFontBold100em##n##pt7b;
FLIGHTUI_ECAM_VLW_SLOTS(FUI_DECL_VLW)
FLIGHTUI_ECAM_GFX_REGULAR_SLOTS(FUI_DECL_GFXR)
FLIGHTUI_ECAM_GFX_BOLD_SLOTS(FUI_DECL_GFXB)
#undef FUI_DECL_VLW
#undef FUI_DECL_GFXR
#undef FUI_DECL_GFXB

// Log a warning per slot still on its stand-in font. Returns count. Called by DisplayManager::begin().
int warn_fallback_slots();
}  // namespace flightui::font
