/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef HALF_RIGHT
#define NO_DEBUG_LEDS // must be on top!
#endif

/* Select half side for flashing */
#define HALF_LEFT
/* #define HALF_RIGHT */

/* #define TAPPING_FORCE_HOLD */
#define TAPPING_TERM 175
/* #define ONESHOT_TAP_TOGGLE 5 */
/* #define ONESHOT_TIMEOUT 5000 */

/* Select hand configuration */
#define MASTER_RIGHT

#define USE_SERIAL_PD2

#undef RGBLED_NUM
#define RGBLIGHT_ANIMATIONS
#define RGBLED_NUM 27
#define RGBLIGHT_LIMIT_VAL 120
#define RGBLIGHT_HUE_STEP 10
#define RGBLIGHT_SAT_STEP 17

/* Right half with trackpad configuration */
#ifdef HALF_RIGHT

/* trackpad connector PCB version */
// 1 - PCB that supports flex caple and the trackpad sensor is mounted on an 'H' keycap
//   - https://github.com/vlukash/corne-trackpad/tree/master/connector
// 2 - PCB woth no flex option, track sensor mounted directly on the PCB
//   - https://github.com/vlukash/corne-trackpad/tree/master/connector-no-flex
#define TRACKPAD_CONNECTOR_VER 2

/* key matrix size */
// Rows are doubled-up
#undef MATRIX_COL_PINS
#define MATRIX_COL_PINS { F4, F5, F6, F7, B7, D5 }

/* ws2812 RGB LED */
#undef RGB_DI_PIN
#define RGB_DI_PIN B5
#endif

#ifdef HALF_RIGHT
#define SSD1306OLED
#endif
