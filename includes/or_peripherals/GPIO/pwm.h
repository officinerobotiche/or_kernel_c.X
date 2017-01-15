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

#ifndef PWM_H
#define	PWM_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/*	INCLUDE																	  */
/******************************************************************************/

#include "or_peripherals/GPIO/gpio.h"
#include <pwm12.h>
    
/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/******************************************************************************/
/*	FUNCTIONS							 									  */
/******************************************************************************/
/**
 * 
 * @param i
 * @param state
 */
void gpio_pwm_enable(unsigned int i, bool state);
/**
 * 
 * @param dutycyclereg
 * @param dutycycle
 * @param updatedisable
 */
void gpio_pwm_set(unsigned int dutycyclereg, unsigned int dutycycle, 
                char updatedisable);
    
#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */

