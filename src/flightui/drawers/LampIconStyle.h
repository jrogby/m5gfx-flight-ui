#pragma once
#include <stdint.h>
#include <string.h>

namespace ui {

// Lamp icon color variants (optional icon left of the label, see AnnunciationBoard::set)
//   LABEL = same black as the label · DARK = lamp's own dark shade (engraved look)
//   WHITE = white · GHOST = label black at ~45 % (subtle)
enum LampIconStyle : uint8_t { ICON_OFF, ICON_LABEL, ICON_DARK, ICON_WHITE, ICON_GHOST, ICON_STYLE_COUNT };
static constexpr const char* LampIconStyleName[ICON_STYLE_COUNT] = {"off", "label", "dark", "white", "ghost"};

inline LampIconStyle lampIconStyleFromName(const char* name) {
    for (uint8_t i = 0; i < ICON_STYLE_COUNT; i++)
        if (name && strcmp(name, LampIconStyleName[i]) == 0) return (LampIconStyle)i;
    return ICON_LABEL;
}

}  // namespace ui
