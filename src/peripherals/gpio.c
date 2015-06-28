/*
 * Copyright (C) 2014-2015 Officine Robotiche
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

#include "peripherals/gpio.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/


/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/
inline void bit_high(hardware_bit_t* bit_control) {
    REGISTER_MASK_SET_HIGH(bit_control->CS_PORT, bit_control->CS_mask);
}

inline void bit_low(hardware_bit_t* bit_control) {
    REGISTER_MASK_SET_LOW(bit_control->CS_PORT, bit_control->CS_mask);
}

inline void bit_toggle(hardware_bit_t* bit_control) {
    REGISTER_MASK_TOGGLE(bit_control->CS_PORT, bit_control->CS_mask);
}

inline bool bit_read(hardware_bit_t* bit_control) {
    // 1. (byte & mask) >> num
    // 2. ( byte >> bitnum) & 0xFE
    return REGISTER_MASK_READ(bit_control->CS_PORT, bit_control->CS_mask);
}