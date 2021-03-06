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

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>

#include "system/soft_timer.h"

#define MICRO 1000000

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

bool init_soft_timer(soft_timer_t *timer, frequency_t frequency, uint32_t time) {
    timer->time = frequency * (time / MICRO);
    timer->counter = 0;
    return true;
}

void reset_timer(soft_timer_t *timer) {
    timer->counter = 0;
}

bool __attribute__((always_inline)) run_timer(soft_timer_t *timer) {
    if ((timer->counter + 1) >= timer->time) {
        timer->counter = 0;
        return true;
    } else {
        timer->counter++;
    }
    return false;
}