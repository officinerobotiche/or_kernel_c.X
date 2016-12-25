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
    
#include "peripherals/gpio.h"
#include "system/events.h"
    
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
    typedef struct led_control {
        gpio_t gpio;
        unsigned int counter;
        unsigned int fr_blink;
        unsigned int wait;
        short number_blink;
    } led_control_t;
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
    /**
     * Initialization LEDs
     * @param led_controller
     * @param len
     * @return event led controller
     */
    hEvent_t LED_Init(uint16_t freq, led_control_t* led_controller, size_t len);
    /**
     * Update frequency or type of blink
     * @param led array of available leds
     * @param num number led
     * @param blink number of blinks
     */
    void LED_updateBlink(led_control_t *led, short num, short blink);
    /**
     * Blink controller for leds. This function you must add in timer function
     * @param led to control
     * @param len number of registered led
     */
    inline void LED_blinkController(led_control_t *led, size_t len);
    /**
     * Start led effect flush
     * @param led_controller list of all leds
     * @param load_blink 
     * @param len number of registered led
     */
    void LED_blinkFlush(led_control_t* led_controller, short* load_blink, size_t len);
    /**
     * Stop all led effects
     * @param led_controller list of all leds
     * @param load_blink
     * @param len number of registered led
     */
    void LED_effectStop(led_control_t* led_controller, short* load_blink, size_t len);


#ifdef	__cplusplus
}
#endif

#endif	/* LED_H */

