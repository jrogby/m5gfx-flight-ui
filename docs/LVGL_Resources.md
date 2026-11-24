# LVGL Resources

## LVGL Grid layouts

https://github.com/lvgl/lvgl/blob/master/examples/layouts/grid/lv_example_grid_1.c


## Font Generation

Some good stuff to know when generating more fonts.

### LVGL Online Font Generator

lvgl.io/tools/fontconverter


| SIGNs                     | HoneywellMCDU   | HoneywellMCDUSmall | ECAMFontRegular   | FBW Display-EIS |
| ------------------------- | --------------- | ------------------ | ----------------- | --------------- |
| nDef BASIC-LATIN-START    |                 | 0x00               | 0x00              | 0x00            |
| uni0009                   |                 |                    |                   | 0x09 (SPACE)    |
| nMarkRet                  |                 |                    | 0x0D              |                 |
| SPACE                     |                 |                    | 0x20              | 0x20            |
| !                         | 0x21            | ?                  | 0x21              | ?               |
| "                         | 0x22            | 0x22               | 0x22              |                 |
| #                         | 0x23            |                    | 0x23              |                 |
| $                         | 0x24            |                    | 0x24              |                 |
| %                         | 0x25            | 0x25               | 0x25              | 0x25            |
| &                         |                 |                    | 0x26              | 0x26            |
| '                         |                 |                    | 0x27              |                 |
| ( ) * +                   | 0x28 - 0x2B     | 0x28 - 0x2B        | 0x28 - 0x2B       | 0x28 - 0x2B     |
| ,                         |                 |                    | 0x2C              |                 |
| - . /                     | 0x2D - 0x2F     | 0x2D - 0x2F        | 0x2D - 0x2F       | 0x2D - 0x2F     |
| 0123456789               | 0x30 - 0x39      | 0x30 - 0x39        | 0x30 - 0x39       | 0x30 - 0x39     |
| :                         | -               | 0x3A               | 0x3A              | 0x3A            |
| ;                         |                 |                    | 0x3B (small >)    | 0x3B (:)        |
| < = >                     |                 |                    | 0x3C - 0x3E       |                 |
| @                         |                 |                    | 0x40 (∆)          | 0x40 (∆)        |
| A - Z                     | 0x41 - 0x5A     | 0x41 - 0x5A        | 0x41 - 0x5A       | 0x41 - 0x5A     |
| [                         | 0x5B            | 0x5B               | 0x5B              |                 |
| /                         | 0x5C            | 0x5C               | 0x5C              |                 |
| ]                         | 0x5D            | 0x5D               | 0x5D              |                 |
| _                         |                 |                    | 0x5F (□)          |                 |
| a - z                     |                 |                    | 0x61 - 0x7A (CAP) | 0x73 only (s)   |
| { PIPE }                  |                 |                    | 0x7B - 0x7D       |                 |
| 0x7F     BASIC-LATIN-STOP |                 |                    |                   |                 |
| 0x80         LATIN1-START |                 |                    |                   |                 |
| uni0085                   |                 |                    |                   | 0x85 empty      |
| NoBreakSpace              |                 |                    |                   | 0xA0            |
| ° DEGREE                  | 0xB0            | 0xB0               | 0xB0              | 0xB0            |
| 0xFF          LATIN1-STOP |                 |                    |                   |                 |
| ← ↑ → ↓                   | 0x2190 - 0x2193 | 0x2190 - 0x2193    | 0x2190 - 0x2193   |                 |
| ∆                         |                 |                    | 0x2206            |                 |
| □ WhiteSquare             |                 |                    |                   | 0x25A1          |
| ▯ WhiteVerticalSquare     |                 |                    |                   | 0x              |

0x00..0x7F; Basic Latin
0x80..0xFF; Latin-1 Supplement
