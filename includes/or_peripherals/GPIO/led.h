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

#ifndef LED_H
#define	LED_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
    
#include "or_peripherals/GPIO/gpio.h"
#include "or_system/events.h"
    
/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
    /// Set High a led
    #define LED_ALWAYS_HIGH -1
    /// Set off led
    #define LED_OFF 0

    /**
     * Struct to control blink led
     * - port name to bit register to mount led
     * - counter to control blink led
     * - number of blink in a period, if:
     *      -# -1 fixed led - LED_ALWAYS_HIGH
     *      -# 0 led off - LED_OFF
     *      -# n number of blink
     */
    typedef struct _LED {
        gpio_t gpio;
        unsigned int counter;
        unsigned int fr_blink;
        unsigned int wait;
        short number_blink;
    } LED_t;
    // Initialization change notification pin
    #define GPIO_LED(x, n) { GPIO_INIT_OUT(x, n), 0, 0, 0, 0 }

    typedef struct _LED_controller {
        // The LED array
        LED_t *leds;
        // The size of LED array
        size_t size;
        // Frequency to execution
        frequency_t freq_cqu;
        // If led effect running
        bool led_effect;
        // If first launch of effect
        bool first;
    } LED_controller_t;
    // Initialization change notification pin
    #define LED_CONTROLLER(led, size, frq) { led, size, frq, false, true}
    
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
    /**
     * Initialization LEDs
     * @param led_controller
     * @param len
     */
    void LED_Init(LED_controller_t *controller);
    /**
     * Update frequency or type of blink
     * @param led array of available leds
     * @param num number led
     * @param blink number of blinks
     */
    void LED_updateBlink(LED_controller_t *controller, short num, short blink);
    /**
     * Start led effect flush
     * @param led_controller list of all leds
     * @param load_blink 
     * @param len number of registered led
     */
    void LED_blinkFlush(LED_controller_t *controller, int* load_blink);
    /**
     * Stop all led effects
     * @param led_controller list of all leds
     * @param load_blink
     * @param len number of registered led
     */
    void LED_effectStop(LED_controller_t *controller, int* load_blink);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

