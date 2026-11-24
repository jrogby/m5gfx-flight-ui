#pragma once
#include "flightui/fonts/slots.h"

// GFX font slots, unqualified (filled by a font pack, see slots.h)
#define FUI_USE_GFXR(n) using flightui::font::ECAMFontRegular##n##pt7b;
#define FUI_USE_GFXB(n) using flightui::font::ECAMFontBold100em##n##pt7b;
FLIGHTUI_ECAM_GFX_REGULAR_SLOTS(FUI_USE_GFXR)
FLIGHTUI_ECAM_GFX_BOLD_SLOTS(FUI_USE_GFXB)
#undef FUI_USE_GFXR
#undef FUI_USE_GFXB
