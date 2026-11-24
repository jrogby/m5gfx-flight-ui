#pragma once
#include <M5GFX.h>

namespace ui {
static constexpr const char* NA = "NA";  // Not available / invalid data indicator

namespace color {
using namespace lgfx::v1;

/**
 *  Base color levels for mixing (1-254 to avoid pure black or white which can be hard to see on some displays)
 */
// Red bases for color mixing
static constexpr uint8_t BASE_R_OFF  = 1;
static constexpr uint8_t BASE_R_LOW  = 76;
static constexpr uint8_t BASE_R_MID  = 153;
static constexpr uint8_t BASE_R_HIGH = 205;
static constexpr uint8_t BASE_R_FULL = 254;

// Green bases for color mixing
static constexpr uint8_t BASE_G_OFF  = 1;
static constexpr uint8_t BASE_G_LOW  = 41;
static constexpr uint8_t BASE_G_MID  = 153;
static constexpr uint8_t BASE_G_HIGH = 206;
static constexpr uint8_t BASE_G_FULL = 254;

// Blue bases for color mixing
static constexpr uint8_t BASE_B_OFF  = 1;
static constexpr uint8_t BASE_B_LOW  = 51;
static constexpr uint8_t BASE_B_MID  = 154;
static constexpr uint8_t BASE_B_HIGH = 235;
static constexpr uint8_t BASE_B_FULL = 254;



// General
static constexpr u16 OFFWHITE = TFT_LIGHTGRAY; // color565(211,211,211)
static constexpr u16 SKYBLUE = color565(BASE_R_MID,BASE_G_HIGH,BASE_B_HIGH); // color565(135,206,235)

// Colorschema for message TX/RX stats
static constexpr u16 TX_SENT          = color565(BASE_R_LOW,  BASE_G_MID, BASE_B_OFF);
static constexpr u16 TX_FAILED        = color565(BASE_R_MID,  BASE_G_OFF, BASE_B_OFF);
static constexpr u16 RX_PARSED        = color565(BASE_R_FULL, BASE_G_MID, BASE_B_LOW);
static constexpr u16 RX_VALID         = color565(BASE_R_FULL, BASE_G_MID, BASE_B_LOW);
static constexpr u16 RX_INVALID       = color565(BASE_R_HIGH, BASE_G_LOW, BASE_B_LOW);

// Airbus ECAM & EFIS style
static constexpr u16 ECAM_TITLE       = color565(BASE_R_HIGH, BASE_G_HIGH, BASE_B_HIGH);
static constexpr u16 ECAM_LABEL       = color565(BASE_R_HIGH, BASE_G_HIGH, BASE_B_HIGH);

// R= 157-163  G= 239-247 B= 126-135
static constexpr u16 ECAM_VALUE       = color565(BASE_R_LOW, BASE_G_FULL, BASE_B_LOW);
static constexpr u16 ECAM_UNIT        = color565(BASE_R_LOW, BASE_G_FULL, BASE_B_FULL);
static constexpr u16 ECAM_BG          = TFT_BLACK;
static constexpr u16 ECAM_ELEC_LABEL  = TFT_GREEN;
static constexpr u16 ECAM_ELEC_BG     = TFT_DARKGRAY;

static constexpr u16 METAR_TEXT       = color565(BASE_R_LOW, BASE_G_FULL, BASE_B_LOW);

static constexpr u16 BOEING_747_RED   = color565(181,109,121);
static constexpr u16 BOEING_747_GREEN = color565(184,241,175);
static constexpr u16 BOEING_747_AMBER = color565(247,197,116);
static constexpr u16 BOEING_AMBER     = color565(245,193,69);

static constexpr u16 AIRBUS_BLUE   = color565(BASE_R_LOW, BASE_G_HIGH, BASE_B_FULL);
static constexpr u16 AIRBUS_GREEN  = color565(BASE_R_OFF, BASE_G_FULL, BASE_B_OFF);
static constexpr u16 AIRBUS_PURPLE = color565(BASE_R_FULL,BASE_G_OFF , BASE_B_FULL);
static constexpr u16 AIRBUS_ORANGE = color565(242,158,52);
static constexpr u16 AIRBUS_YELLOW = color565(BASE_R_FULL,BASE_G_FULL,BASE_B_OFF);
static constexpr u16 AIRBUS_RED    = color565(BASE_R_FULL,BASE_G_OFF,BASE_B_OFF);

constexpr u16 LAMP_ERROR   = AIRBUS_RED;         // color565(228,50,22);   // Red
constexpr u16 LAMP_OK      = AIRBUS_GREEN;       // color565(98,235,65);   // Green
constexpr u16 LAMP_WARNING = AIRBUS_ORANGE;      // color565(245,196,64);  // Yellow/Amber
constexpr u16 LAMP_PENDING = AIRBUS_BLUE;        // color565(100,235,232); // Blue = pending operation
constexpr u16 LAMP_NA      = color565(53,52,53); //                           Gray = NA/Off
constexpr u16 LAMP_LABEL   = TFT_BLACK;

// Precomputed dark variants for lamp gradient (10% brightness)
constexpr u16 darken565(u16 c) { return (((c >> 11) & 0x1F) / 10 << 11) | ((((c >> 5) & 0x3F) / 10) << 5) | ((c & 0x1F) / 10); }
constexpr u16 LAMP_ERROR_DK   = darken565(LAMP_ERROR);
constexpr u16 LAMP_OK_DK      = darken565(LAMP_OK);
constexpr u16 LAMP_WARNING_DK = darken565(LAMP_WARNING);
constexpr u16 LAMP_PENDING_DK = darken565(LAMP_PENDING);
constexpr u16 LAMP_NA_DK      = darken565(LAMP_NA);
constexpr u16 LAMP_NA_DARK    = darken565(LAMP_NA);

} // namespace color
} // namespace ui