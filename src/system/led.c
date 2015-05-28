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

#include "system/led.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

bool led_effect = false;
bool first = true;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void LED_Init(led_control_t* led_controller, size_t len) {
    int i;
    for (i = 0; i < len; ++i) {
        led_controller[i].CS_mask = 1 << led_controller[i].pin->CS_pin;
        led_controller[i].wait = 0;
        LED_updateBlink(i, LED_OFF);
    }
}

void LED_updateBlink(led_control_t* led_controller, short num, short blink) {
    led_controller[num].number_blink = blink;
    switch (led_controller[num].number_blink) {
        case LED_OFF:
            //Clear bit - Set to 0
            *(led_controller[num].pin->CS_PORT) &= ~led_controller[num].CS_mask;
            break;
        case LED_ALWAYS_HIGH:
            //Set bit - Set to 1
            *(led_controller[num].pin->CS_PORT) |= led_controller[num].CS_mask;
            break;
        default:
            led_controller[num].fr_blink = SYSTEM_FREQ / (2 * led_controller[num].number_blink);
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

inline void LED_blinkController(led_control_t *led) {
    if (led->counter > led->wait && led->counter < SYSTEM_FREQ) {
        if (led->counter % led->fr_blink == 0) {
            //Toggle bit
            *(led->pin->CS_PORT) ^= led->CS_mask;
        }
        led->counter++;
    } else if (led->counter >= 3 * SYSTEM_FREQ / 2) {
        led->counter = 0;
    } else {
        //Clear bit - Set to 0
        *(led->pin->CS_PORT) &= ~led->CS_mask;
        led->counter++;
    }
}

void LED_blinkFlush(led_control_t* led_controller, short* load_blink, size_t len) {
    int i;
    for (i = 0; i < len; ++i) {
        led_controller[i].wait = i * ((float) SYSTEM_FREQ / len);
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
