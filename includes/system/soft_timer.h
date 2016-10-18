/*
 * Copyright (C) 2014-2016 Officine Robotiche
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

#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
    
#include "events.h"
    
/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
    typedef struct _timer {
        uint32_t time;
        uint32_t counter;
    } soft_timer_t;
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

    /**
     * Initialize a soft timer. This timer works in a real time routine
     * @param timer the timer
     * @param frequency main frequency
     * @param time timer to set the timer
     * @return return true if registered
     */
    bool init_soft_timer(soft_timer_t *timer, frequency_t frequency, uint16_t time);
    
    /**
     * Reset the timer to zero
     * @param timer the timer
     */
    void reset_timer(soft_timer_t *timer);
    
    /**
     * Run the timer and check if is in time
     * @param timer the timer
     * @return return true if is in time
     */
    bool run_timer(soft_timer_t *timer);
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

