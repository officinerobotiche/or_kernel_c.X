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

#include "or_peripherals/GPIO/led.h"
#include "or_system/task_manager.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

/**
 * Tc -> counter = 1sec = 1000 interrupts
 * !       Tc/2        !   Tc/2       !
 * !     !_____   _____!              !
 * !     !|   |   |   |!              !
 * !-----!|   |---|   |! . . . -------!
 * !     !             !              !
 * ! WAIT   Tc/2-WAIT  !   Tc/2       !
 */

inline void LED_blinkController(int argc, int* argv) {
    LED_controller_t *controller = (LED_controller_t *) argv[0];
    unsigned int i;
    for(i = 0; i < controller->size; ++i) {
        if (controller->leds[i].number_blink > LED_OFF) {
            if (controller->leds[i].counter > controller->leds[i].wait && 
                    controller->leds[i].counter < controller->freq_cqu) {
                if (controller->leds[i].counter % controller->leds[i].fr_blink == 0) {
                    //Toggle bit
                    REGISTER_MASK_TOGGLE(controller->leds[i].gpio.CS_PORT, 
                            controller->leds[i].gpio.CS_mask);
                }
                controller->leds[i].counter++;
            } else if (controller->leds[i].counter >= 3 * controller->freq_cqu / 2) {
                controller->leds[i].counter = 0;
            } else {
                //Clear bit - Set to 0
                REGISTER_MASK_SET_LOW(controller->leds[i].gpio.CS_PORT, 
                        controller->leds[i].gpio.CS_mask);
                controller->leds[i].counter++;
            }
        }
    }
}

void LED_Init(LED_controller_t *controller) {
    unsigned int i;
    // Initialize all LED and set OFF
    for (i = 0; i < controller->size; ++i) {
        gpio_init_pin(&controller->leds[i].gpio);
        LED_updateBlink(controller, i, LED_OFF);
    }
    /// Register event
    hEvent_t LED_service_handle = register_event(&LED_blinkController);
    // Register task
    hTask_t LED_task_handle = task_load_data(LED_service_handle, controller->freq_cqu, 1, controller);
    /// Run task controller
    task_set(LED_task_handle, RUN);
}

void LED_updateBlink(LED_controller_t *controller, short num, short blink) {
    controller->leds[num].number_blink = blink;
    switch (controller->leds[num].number_blink) {
        case LED_OFF:
            //Clear bit - Set to 0
            REGISTER_MASK_SET_LOW(controller->leds[num].gpio.CS_PORT, 
                    controller->leds[num].gpio.CS_mask);
            break;
        case LED_ALWAYS_HIGH:
            //Set bit - Set to 1
            REGISTER_MASK_SET_HIGH(controller->leds[num].gpio.CS_PORT, 
                    controller->leds[num].gpio.CS_mask);
            break;
        default:
            controller->leds[num].fr_blink = controller->freq_cqu / (2 * controller->leds[num].number_blink);
            break;
    }
    controller->leds[num].counter = 0;
}

void LED_blinkFlush(LED_controller_t *controller, int* load_blink) {
    int i;
    for (i = 0; i < controller->size; ++i) {
        controller->leds[i].wait = i * ((float) controller->freq_cqu / controller->size);
        load_blink[i] = controller->leds[i].number_blink;
        LED_updateBlink(controller, i, 1);
    }
    controller->led_effect = true;
}

void LED_effectStop(LED_controller_t *controller, int* load_blink) {
    int i;
    int value = 0;
    if (controller->led_effect) {
        for (i = 0; i < controller->size; ++i) {
            value += controller->leds[i].counter;
        }
        if (value == 0) {
            if (~controller->first) {
                for (i = 0; i < controller->size; ++i) {
                    LED_updateBlink(controller, i, load_blink[i]);
                    controller->leds[i].wait = 0;
                }
                controller->led_effect = false;
                controller->first = true;
            } else {
                controller->first = false;
            }
        }
    }
}
