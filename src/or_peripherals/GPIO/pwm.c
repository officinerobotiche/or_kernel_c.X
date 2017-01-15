/*
 * Copyright (C) 2014-2017 Officine Robotiche
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

#include "or_peripherals/GPIO/pwm.h"

#define LNG_PWM_GENERATOR 2

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

bool PWM[LNG_PWM_GENERATOR];

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

void gpio_pwm_init() {
    unsigned int i;
    // If all PWM are disable switch off the PWM controller
    for(i = 0; i < LNG_PWM_GENERATOR; ++i) {
        PWM[i] = false;
    }
}

void gpio_pwm_enable(unsigned int Idx, bool state) {
    unsigned int i;
    // set the new state
    PWM[Idx] = state;
    bool status = true;
    // If all PWM are disable switch off the PWM controller
    for(i = 0; i < LNG_PWM_GENERATOR; ++i) {
        status |= PWM[i];
    }
    // set the new state 
    PTCONbits.PTEN = state;
}

void gpio_pwm_set(unsigned int dutycyclereg, unsigned int dutycycle, 
                char updatedisable) {
    SetDCMCPWM1(dutycyclereg, dutycycle, updatedisable);
}