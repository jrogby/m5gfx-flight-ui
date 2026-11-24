#pragma once
#include "flightui/fonts/slots.h"

// ECAMFontRegular VLW font slots, unqualified (filled by a font pack, see slots.h)
#define FUI_USE_VLW(n) using flightui::font::ECAMFontRegular##n;
FLIGHTUI_ECAM_VLW_SLOTS(FUI_USE_VLW)
#undef FUI_USE_VLW
