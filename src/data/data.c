/*
 * Copyright (C) 2015 Officine Robotiche
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

/*****************************************************************************/
/* Files to Include                                                          */
/*****************************************************************************/

#include "data/data.h"

/*****************************************************************************/
/* Global Variable Declaration                                               */
/*****************************************************************************/

/*****************************************************************************/
/* User Functions                                                            */
/*****************************************************************************/

void inline protectedMemcpy(hardware_bit_t* reg, void *destination, const void *source, size_t num) {
    if (bit_read(reg)) {
        bit_low(reg);
        memcpy(destination, source, num);
        bit_high(reg);
    } else {
        memcpy(destination, source, num);
    }
}