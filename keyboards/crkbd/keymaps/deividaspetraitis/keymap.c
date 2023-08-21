/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

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

#include QMK_KEYBOARD_H
#include "bootloader.h"

#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif

extern bool isScrollMode;

#ifdef HALF_RIGHT
#include "mousekey.h"
#include "pointing_device.h"
#include "report.h"
#endif


#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

int cur_dance (tap_dance_state_t *state);
void shift_finished (tap_dance_state_t *state, void *user_data);
void shift_reset (tap_dance_state_t *state, void *user_data);
void raise_finished (tap_dance_state_t *state, void *user_data);
void raise_reset (tap_dance_state_t *state, void *user_data);
void dance_cln_finished (tap_dance_state_t *state, void *user_data);
void dance_cln_reset (tap_dance_state_t *state, void *user_data);

typedef struct {
  bool is_press_action;
  int state;
} tap;

enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2,
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  TRIPLE_TAP = 5,
  TRIPLE_HOLD = 6
};

// Tap Dance declarations
enum {
  TD_LSFT = 0,
  TD_RAISE,
  TD_TEST,
  CT_CLN,
  CT_LBRC,
  CT_RBRC
};

enum layer_names {
  _DVORAK,
  _LOWER,
  _RAISE,
  _RAISE_TD0, // raise one shot
  _SHIFT_TD, // one shot
  _ADJUST
};

enum custom_keycodes {
  DVORAK = SAFE_RANGE,
  SHIFT,
  LOWER,
  RAISE,
  RGBRST,
  MBTN1,
  MBTN2,
  BTCK,
  MSCRL
};

#define KC_LOWER     LOWER
#define KC_RAISE     RAISE
#define KC_BTCK      BTCK
#define KC_MSCRL     MSCRL
#define KC_MBTN1     MBTN1
#define KC_MBTN2     MBTN2

#define KC_RST   QK_BOOT
#define KC_LRST  RGBRST
#define KC_LTOG  RGB_TOG
#define KC_LHUI  RGB_HUI
#define KC_LHUD  RGB_HUD
#define KC_LSAI  RGB_SAI
#define KC_LSAD  RGB_SAD
#define KC_LVAI  RGB_VAI
#define KC_LVAD  RGB_VAD
#define KC_LMOD  RGB_MOD


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_DVORAK] = LAYOUT_split_3x6_3(
  //,------------------------------------------------------------.                    ,-----------------------------------------------------.
     KC_LGUI,        TD(CT_CLN), KC_COMM, KC_DOT,  KC_P,    KC_Y,                         KC_F,    KC_G,    KC_C,    KC_R,    KC_L,    KC_BSPC,
  //|---------------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     XXXXXXX,        KC_A,    KC_O,    KC_E,    KC_U,    KC_I,                         KC_D,    KC_H,    KC_T,    KC_N,    KC_S,    KC_BSLS,
  //|---------------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     TD(TD_LSFT),    KC_QUOT, KC_Q,    KC_J,    KC_K,    KC_X,                         KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,    KC_SLSH,
  //|---------------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_LOWER, CTL_T(KC_ESC), SFT_T(KC_SPC),     KC_ENT, TD(TD_RAISE), KC_RALT
                                      //`---------------------------------'  `--------------------------'
  ),

  [_LOWER] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                          ,-----------------------------------------------------.
     KC_7,    KC_5,    KC_3,    KC_1,    KC_9,    KC_0,                               KC_2,    KC_4,    KC_6,    KC_8,    KC_RGHT, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                          |--------+--------+--------+--------+--------+--------|
     KC_BTCK, XXXXXXX, KC_MSCRL,KC_MBTN1,KC_MBTN2,XXXXXXX,                            XXXXXXX, KC_LEFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                          |--------+--------+--------+--------+--------+--------|
     XXXXXXX, XXXXXXX, XXXXXXX, KC_DOWN, KC_UP,   XXXXXXX,                            XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_MINS, KC_TAB,
  //|--------+--------+--------+--------+--------+-----------------+------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, CTL_T(KC_ESC),  SFT_T(KC_SPC),     KC_ENT,   TD(TD_RAISE), KC_RALT
                                      //`--------------------------'            `--------------------------'
  ),

  [_RAISE] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     XXXXXXX, KC_AMPR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_EQL,  KC_ASTR, XXXXXXX, KC_PLUS, KC_DLR,  KC_EXLM,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_TILD, XXXXXXX, XXXXXXX, TD(CT_RBRC), TD(CT_LBRC), XXXXXXX,                      KC_PERC, KC_CIRC, XXXXXXX, XXXXXXX, KC_AT,   KC_HASH,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     XXXXXXX, XXXXXXX, XXXXXXX, KC_RPRN, KC_LPRN, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_LOWER,CTL_T(KC_ESC),SFT_T(KC_SPC),     KC_ENT,  _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  [_RAISE_TD0] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_NO,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     XXXXXXX, XXXXXXX, XXXXXXX, TD(TD_TEST), KC_LBRC, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_LOWER,CTL_T(KC_ESC),SFT_T(KC_SPC),     KC_ENT,  _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),
  [_SHIFT_TD] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_END,  XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     KC_NO,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, KC_HOME, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                         KC_LOWER,CTL_T(KC_ESC),SFT_T(KC_SPC),     KC_ENT,  _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  [_ADJUST] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_RST, KC_LRST, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LOWER, _______,  SFT_T(KC_SPC),     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  )
};

int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
}

#ifdef OLED_ENABLE

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

bool oled_task_user(void) {
  if (is_keyboard_master()) {
    // If you want to change the display of OLED, you need to change here
    oled_write_ln(read_layer_state(), false);
    oled_write_ln(read_keylog(), false);
    oled_write_ln(read_keylogs(), false);
  } else {
    oled_write(read_logo(), false);
  }
  return false;
}

#endif


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    #ifdef OLED_ENABLE
    set_keylog(keycode, record);
    #endif
  }

  #ifdef HALF_RIGHT
  report_mouse_t currentReport = {};
  #endif

  switch (keycode) {
    case DVORAK:
      if (record->event.pressed) {
        persistent_default_layer_set(1UL<<_DVORAK);
      }
      return false;
    case KC_LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
	  return false;
    case KC_RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
	// Write backtick mark on KC_BTCK button press
    case KC_BTCK:
	  if (record->event.pressed) {
		  SEND_STRING("`");
	  }
	  return false;
	
	// Handle trackpoint
	#ifdef HALF_RIGHT
	// Handle KC_MBTN1 as left mouse button
    case KC_MBTN1:
      currentReport = pointing_device_get_report();
      if (record->event.pressed) {
        currentReport.buttons |= MOUSE_BTN1;
      }
      else {
        currentReport.buttons &= ~MOUSE_BTN1;
      }
      pointing_device_set_report(currentReport);
      pointing_device_send();

      return false;
	// Handle KC_MBTN2 as right mouse button
    case KC_MBTN2:
      currentReport = pointing_device_get_report();
      if (record->event.pressed) {
        currentReport.buttons |= MOUSE_BTN2;
      }
      else {
        currentReport.buttons &= ~MOUSE_BTN2;
      }
      pointing_device_set_report(currentReport);
      pointing_device_send();

      return false;
	// Enable mouse scroll when KC_MSCRL is pressed
	case KC_MSCRL:
	  if (record->event.pressed) {
		  isScrollMode = true;
	  }
	  else {
		  isScrollMode = false;
	  }
	  return false;
	#endif
    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;
	case KC_NO:
		if (record->event.pressed && is_oneshot_layer_active()) {
		  clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
		  return true;
		}
	  break;
  }
  return true;
}

int cur_dance (tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->pressed) return SINGLE_HOLD;
    else return SINGLE_TAP;
  }
  else if (state->count == 2) {
    if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  else if (state->count == 3) {
    if (state->interrupted || !state->pressed)  return TRIPLE_TAP;
    else return TRIPLE_HOLD;
  }
  else return 8;
}

static tap shifttap_state = {
  .is_press_action = true,
  .state = 0
};

static tap raise_state = {
  .is_press_action = true,
  .state = 0
};

void shift_finished (tap_dance_state_t *state, void *user_data) {
  shifttap_state.state = cur_dance(state);
  switch (shifttap_state.state) {
	case SINGLE_TAP: set_oneshot_layer(_SHIFT_TD, ONESHOT_START); clear_oneshot_layer_state(ONESHOT_PRESSED); break;
	/* case SINGLE_HOLD: register_code(KC_LSFT); break; */
  }
}

void shift_reset (tap_dance_state_t *state, void *user_data) {
  switch (shifttap_state.state) {
    case SINGLE_TAP: break;
    /* case SINGLE_HOLD: unregister_code(KC_LSFT); break; */
  }
  shifttap_state.state = 0;
}

void raise_finished (tap_dance_state_t *state, void *user_data) {
  raise_state.state = cur_dance(state);
  switch (raise_state.state) {
	case SINGLE_HOLD:
		layer_on(_RAISE); // for a layer-tap key, use `layer_on(_MY_LAYER)` here
	break;
	case DOUBLE_HOLD:
		layer_on(_RAISE_TD0); // for a layer-tap key, use `layer_on(_MY_LAYER)` here
	break;
  }
}

void raise_reset (tap_dance_state_t *state, void *user_data) {
  switch (raise_state.state) {
	case SINGLE_HOLD:
		layer_off(_RAISE); // for a layer-tap key, use `layer_on(_MY_LAYER)` here
	break;
	case DOUBLE_HOLD:
		layer_off(_RAISE_TD0); // for a layer-tap key, use `layer_on(_MY_LAYER)` here
	break;
  }
  raise_state.state = 0;
}

void test_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code (KC_LBRC);
  } else {
	register_code (KC_RSFT);
    register_code (KC_LBRC);
  }
}

void test_reset (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code (KC_LBRC);
  } else {
	unregister_code (KC_RSFT);
    unregister_code (KC_LBRC);
  }
}

void dance_cln_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code (KC_SCLN);
  } else {
	register_code (KC_RSFT);
    register_code (KC_SCLN);
  }
}

void dance_cln_reset (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code (KC_SCLN);
  } else {
	unregister_code (KC_RSFT);
    unregister_code (KC_SCLN);
  }
}

void dance_lbcr_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code (KC_LBRC);
  } else {
	register_code (KC_RSFT);
    register_code (KC_LBRC);
  }
}

void dance_lbrc_reset (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code (KC_LBRC);
  } else {
	unregister_code (KC_RSFT);
    unregister_code (KC_LBRC);
  }
}

void dance_rbcr_finished (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    register_code (KC_RBRC);
  } else {
	register_code (KC_RSFT);
    register_code (KC_RBRC);
  }
}

void dance_rbrc_reset (tap_dance_state_t *state, void *user_data) {
  if (state->count == 1) {
    unregister_code (KC_RBRC);
  } else {
	unregister_code (KC_RSFT);
    unregister_code (KC_RBRC);
  }
}

tap_dance_action_t tap_dance_actions[] = {
  [TD_LSFT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, shift_finished, shift_reset),
  [TD_RAISE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, raise_finished, raise_reset),
  [TD_TEST] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, test_finished, test_reset),
  [CT_CLN] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_cln_finished, dance_cln_reset),
  [CT_LBRC] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_lbcr_finished, dance_lbrc_reset),
  [CT_RBRC] = ACTION_TAP_DANCE_FN_ADVANCED (NULL, dance_rbcr_finished, dance_rbrc_reset),
};
