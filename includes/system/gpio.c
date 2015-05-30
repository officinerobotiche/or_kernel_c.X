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

#include "system/gpio.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/


/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

inline void bit_setup(bit_control_t* bit_control) {
    bit_control->CS_mask = 1 << bit_control->pin->CS_pin;
}

inline void bit_high(bit_control_t* bit_control) {
    *(bit_control->pin->CS_PORT) |= bit_control->CS_mask;
}

inline void bit_low(bit_control_t* bit_control) {
    *(bit_control->pin->CS_PORT) &= ~bit_control->CS_mask;
}

inline void bit_toggle(bit_control_t* bit_control) {
    *(bit_control->pin->CS_PORT) ^= bit_control->CS_mask;
}

inline bool bit_read(bit_control_t* bit_control) {
    // or => ( byte >> bitnum) & 0xFE
    return (bit_control->pin->CS_PORT & bit_control->CS_mask) >> bit_control->pin->CS_pin;
}