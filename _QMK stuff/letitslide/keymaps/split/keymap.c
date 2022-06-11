/* Copyright 2021 drhigsby
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

bool is_gui_tab_active = false; // ADD this near the begining of keymap.c
bool is_winos = true; //Controls slider volume behavior allowing toggle between Win & Mac
uint16_t gui_tab_timer = 0;     // we will be using them soon.
uint16_t gui_tab_delay = 1000;  // amount of time to keep mod "held"

enum my_keycodes {
  PRVAPP = SAFE_RANGE,
  NXTAPP,
  VOLTOG
};

enum layers{
  _BASE,
  _NUMNAV,
  _FN
};


#define NUMNAV MO(_NUMNAV)
#define FN MO(_FN)
#define PUSHSPIN LT(_NUMNAV, KC_BSPC)
#define zzzz KC_TRNS

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_225u_2u(
        KC_GESC, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, PUSHSPIN,
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT,
        KC_LSPO, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSPC, KC_UP,
        KC_LGUI, KC_LALT, KC_LCTL, KC_PGDN, NUMNAV, KC_SPC, KC_PGUP, FN, VOLTOG, KC_LEFT, KC_DOWN, KC_RGHT
  ),

    [_NUMNAV] = LAYOUT_225u_2u(
        KC_GRV,    KC_1,  KC_2,    KC_3,   KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_MINS,  KC_EQL,  KC_DEL,
        KC_CAPS, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PIPE, KC_BSLS,
        zzzz,    zzzz,  zzzz,    zzzz,   zzzz,    zzzz,    zzzz,    zzzz,    zzzz,    zzzz,    zzzz,    zzzz, KC_HOME,
        zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, KC_PGUP, KC_END, KC_PGDN
  ),

    [_FN] = LAYOUT_225u_2u(
        zzzz, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, zzzz,
        zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz,
        zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz,
        zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, zzzz
  )

};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case NXTAPP:
      if (record->event.pressed) {
        if (!is_gui_tab_active) {
            is_gui_tab_active = true;
            is_winos ? register_code(KC_LALT) : register_code(KC_LGUI);
        }
        gui_tab_timer = timer_read();
        register_code(KC_TAB);
      } else {
        is_winos ? unregister_code(KC_LALT) : unregister_code(KC_LGUI);
      }
      return false; // Skip all further processing of this key
    case PRVAPP:
      if (record->event.pressed) {
        if (!is_gui_tab_active) {
            is_gui_tab_active = true;
            is_winos ? register_code(KC_LALT) : register_code(KC_LGUI);
        }
        gui_tab_timer = timer_read();
        register_code(KC_LSFT);
        is_winos ? register_code(KC_TAB) : register_code(KC_GRAVE);
      } else {
        unregister_code(KC_LSFT);
        if(is_winos) {
            unregister_code(KC_TAB);
            unregister_code(KC_LALT);
        } else {
            unregister_code(KC_GRAVE);
            unregister_code(KC_LGUI);
        }
      }
      return false; // Skip all further processing of this key
    case VOLTOG:
      if (record->event.pressed) {
        gui_tab_timer = timer_read();
        if(timer_elapsed(gui_tab_timer) < 1000) {
            is_winos = is_winos ? false : true;
        }
      }
      return false; // Skip all further processing of this key
    default:
      return true; // Process all other keycodes normally
  }
}

void matrix_scan_user(void) { // The very important timer.
  if (is_gui_tab_active) {
    if (timer_elapsed(gui_tab_timer) > 1000) {
      unregister_code(KC_LGUI);
      is_gui_tab_active = false;
    }
  }
}

#ifdef ENCODER_ENABLE
    bool encoder_update_user(uint8_t index, bool clockwise) {
        if (index == 0) {
            switch (biton32(layer_state))
            {
                case _BASE:
                    clockwise ? tap_code(KC_PGUP) : tap_code(KC_PGDN);
                    break;
                case _NUMNAV:
                    //Word selection
                    register_code(KC_LSFT);
                    register_code(KC_LCTRL);
                    clockwise ? tap_code(KC_RGHT) : tap_code(KC_LEFT);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTRL);
                    break;
                case _FN:
                    clockwise ? tap_code(KC_VOLU) : tap_code(KC_VOLD);
                    break;
            }
        }
        return true;
    }
#endif
