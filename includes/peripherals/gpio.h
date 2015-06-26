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

#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>         /* For true/false definition                     */

#define BIT_MASK(x)                       (1 << (x))
#define REGISTER_INIT(reg, x)             {&(reg), BIT_MASK(x)}
#define REGISTER_MASK_SET_HIGH(reg, mask) (*(reg) |= (mask))
#define REGISTER_MASK_SET_LOW(reg, mask)  (*(reg) &= ~(mask))
#define REGISTER_MASK_TOGGLE(reg, mask)   (*(reg) ^= (mask))
#define REGISTER_MASK_READ(reg, mask)     ((*(reg) & (mask)) == (mask))
    
    typedef volatile unsigned int * REGISTER;
    
    typedef struct _hardware_bit {
        REGISTER CS_PORT;
        unsigned int CS_mask;
    } hardware_bit_t;
    
    inline void bit_high(hardware_bit_t* bit_control);
    
    inline void bit_low(hardware_bit_t* bit_control);
    
    inline void bit_toggle(hardware_bit_t* bit_control);
    
    inline bool bit_read(hardware_bit_t* bit_control);

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

