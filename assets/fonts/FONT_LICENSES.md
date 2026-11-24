# Font licenses

Library code is MIT (see `library.json`). Bundled fonts keep their own licenses, listed here.
Applies to the source `.vlw` files in this folder and the generated `src/fonts/*` arrays.

| Font              | Files                                | Author                                                               | License                                       | Status                                                      |
|-------------------|--------------------------------------|----------------------------------------------------------------------|-----------------------------------------------|-------------------------------------------------------------|
| Honeywell MCDU    | `HoneywellMCDU*.vlw`                 | Tyler Knox (tyler58546, GitHub / FontStruct), © 2021                 | SIL Open Font License 1.1                     | Confirmed                                                   |
| ECAM Font Regular | moved out to `lib/m5gfx-fonts-ecam` | FlightGear A320-family devs (Josh Davidson, Jonathan Redpath et al.) | GPL-2.0 (aircraft package, no font exception) | **Not in this lib** - slots only (`flightui/fonts/slots.h`) |

## SIL OFL 1.1 obligations (Honeywell MCDU)
- Free to use, modify, embed and redistribute, also inside commercial software.
- Must not be sold by itself.
- The copyright notice and the full OFL 1.1 license text must ship with the font
  -> add the official text as `OFL.txt` next to this file (from the font's source repo or openfontlicense.org).
- Converted/modified versions (VLW, C arrays) stay under OFL 1.1. If the original declares a
  Reserved Font Name, derivatives must not use that name.

## ECAM Font Regular - open
- Origin: FlightGear A320-family aircraft (github.com/legoboyvdlp/A320-family, `Fonts/`; also FGAddon trunk).
  Package is GPL-2.0 (README.copyright.md: "Copyright (c) 2025 Josh Davidson, Jonathan Redpath, merspieler,
  Matthew Maring, Thorsten Herrmann, Semir Gebran"). No separate font license or font exception found (checked 2026-11-02).
- If GPL **without** a font exception: embedding it as C arrays makes it part of the firmware binary,
  so the firmware (and this lib, when distributed with the font) would fall under GPL. That conflicts with the lib's MIT license.
- Decision (2026-11-02): option (b). Font data lives in the separate GPL lib `m5gfx-fonts-ecam`;
  this lib only has named font slots with OFL/built-in fallbacks. App calls `ecam_fonts::install()`.
