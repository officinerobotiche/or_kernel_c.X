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
#include "system/task_manager.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
#define LED "LED"
static string_data_t _MODULE_LED = {LED, sizeof (LED)};

/// If led effect running
bool led_effect = false;
/// If first launch of effect
bool first = true;
/// Frequency to esecution
frequency_t freq_cqu;
/// Led event handle
static hEvent_t LED_service_handle = INVALID_EVENT_HANDLE;
/// Led task handle
static hTask_t LED_task_handle = INVALID_TASK_HANDLE;
/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void serviceLED(int argc, int* argv) {
    LED_blinkController((led_control_t*) argv[0], (size_t) argv[1]);
}

void LED_Init(uint16_t freq, led_control_t* led_controller, size_t len) {
    int i;
    freq_cqu = freq;
    for (i = 0; i < len; ++i) {
        led_controller[i].wait = 0;
        LED_updateBlink(led_controller, i, LED_OFF);
    }
    /// Register module
    hModule_t led_module = register_module(&_MODULE_LED);
    /// Register event
    LED_service_handle = register_event_p(led_module, &serviceLED, EVENT_PRIORITY_LOW);
    
    LED_task_handle = task_load_data(LED_service_handle, freq_cqu, 2, led_controller, len);
    /// Run task controller
    task_set(LED_task_handle, RUN);
}

void LED_updateBlink(led_control_t* led_controller, short num, short blink) {
    led_controller[num].number_blink = blink;
    switch (led_controller[num].number_blink) {
        case LED_OFF:
            //Clear bit - Set to 0
            REGISTER_MASK_SET_LOW(led_controller[num].pin->CS_PORT, led_controller[num].pin->CS_mask);
            break;
        case LED_ALWAYS_HIGH:
            //Set bit - Set to 1
            REGISTER_MASK_SET_HIGH(led_controller[num].pin->CS_PORT, led_controller[num].pin->CS_mask);
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
                    REGISTER_MASK_TOGGLE(led[i].pin->CS_PORT, led[i].pin->CS_mask);
                }
                led[i].counter++;
            } else if (led[i].counter >= 3 * freq_cqu / 2) {
                led[i].counter = 0;
            } else {
                //Clear bit - Set to 0
                REGISTER_MASK_SET_LOW(led[i].pin->CS_PORT, led[i].pin->CS_mask);
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
