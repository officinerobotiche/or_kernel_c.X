/*
 * Copyright (C) 2014 Officine Robotiche
 * Author: Raffaello Bonghi
 * email:  raffaello.bonghi@officinerobotiche.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU Lesser General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "peripherals/led.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

bool led_effect = false;
bool first = true;
uint16_t freq_cqu;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void LED_Init(uint16_t freq, led_control_t* led_controller, size_t len) {
    int i;
    freq_cqu = freq;
    for (i = 0; i < len; ++i) {
        bit_setup(&led_controller[i].pin);
        led_controller[i].wait = 0;
        LED_updateBlink(led_controller, i, LED_OFF);
    }
}

void LED_updateBlink(led_control_t* led_controller, short num, short blink) {
    led_controller[num].number_blink = blink;
    switch (led_controller[num].number_blink) {
        case LED_OFF:
            //Clear bit - Set to 0
            bit_low(&led_controller[num].pin);
            break;
        case LED_ALWAYS_HIGH:
            //Set bit - Set to 1
            bit_high(&led_controller[num].pin);
            break;
        default:
            led_controller[num].fr_blink = freq_cqu / (2 * led_controller[num].number_blink);
            break;
    }
    led_controller[num].counter = 0;
}

/**
 * Tc -> counter = 1sec = 1000 interrupts
 * !       Tc/2        !   Tc/2       !
 * !     !_____   _____!              !
 * !     !|   |   |   |!              !
 * !-----!|   |---|   |! . . . -------!
 * !     !             !              !
 * ! WAIT   Tc/2-WAIT  !   Tc/2       !
 */

inline void LED_blinkController(led_control_t *led, size_t len) {
    short i;
    for(i = 0; i < len; ++i) {
        if (led[i].number_blink > LED_OFF) {
            if (led[i].counter > led[i].wait && led[i].counter < freq_cqu) {
                if (led[i].counter % led[i].fr_blink == 0) {
                    //Toggle bit
                    bit_toggle(&led[i].pin);
                }
                led[i].counter++;
            } else if (led[i].counter >= 3 * freq_cqu / 2) {
                led[i].counter = 0;
            } else {
                //Clear bit - Set to 0
                bit_low(&led[i].pin);
                led[i].counter++;
            }
        }
    }
}

void LED_blinkFlush(led_control_t* led_controller, short* load_blink, size_t len) {
    int i;
    for (i = 0; i < len; ++i) {
        led_controller[i].wait = i * ((float) freq_cqu / len);
        load_blink[i] = led_controller[i].number_blink;
        LED_updateBlink(led_controller, i, 1);
    }
    led_effect = true;
}

void LED_effectStop(led_control_t* led_controller, short* load_blink, size_t len) {
    int i;
    int value = 0;
    if (led_effect) {
        for (i = 0; i < len; ++i) {
            value += led_controller[i].counter;
        }
        if (value == 0) {
            if (~first) {
                for (i = 0; i < len; ++i) {
                    LED_updateBlink(led_controller, i, load_blink[i]);
                    led_controller[i].wait = 0;
                }
                led_effect = false;
                first = true;
            } else {
                first = false;
            }
        }
    }
}
