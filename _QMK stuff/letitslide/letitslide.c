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
#include "letitslide.h"
#include "analog.h"
#include "print.h"

int16_t max = 1023;
int16_t mid = 200;
int16_t max_ticks = 50;
int16_t pot_oldVal = 0;

void matrix_init_user(void) {
    analogReference(ADC_REF_POWER);
    for (int i = 0; i<max_ticks;++i){
        tap_code(KC_VOLD);
    }
}

void matrix_scan_user(void){
    int pot_val   = (analogReadPin(POT_PIN));
    int delta = pot_val - pot_oldVal;
    int margin = pot_val <= mid ?
        round((float)mid/(max_ticks/2))
        : round((float)(max-mid)/(max_ticks/2));

    if (abs(delta) >= margin) {
        int ticks = round((float)delta/margin);
        for (int i = 1; i<=abs(ticks);++i){
            ticks > 0 ? tap_code(KC_VOLU) : tap_code(KC_VOLD);
        }
        pot_oldVal = pot_val;
    }
}
