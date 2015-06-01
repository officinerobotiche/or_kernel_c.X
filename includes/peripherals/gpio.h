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

    typedef volatile unsigned int * REGISTER;
    
    typedef struct _hardware_bit {
        REGISTER CS_PORT;
        const unsigned int CS_pin;
    } hardware_bit_t;
    
    typedef struct _bit_control {
        hardware_bit_t * pin;
        unsigned int CS_mask;
    } bit_control_t;
    
    inline void bit_setup(bit_control_t* bit_control);
    
    inline void bit_high(bit_control_t* bit_control);
    
    inline void bit_low(bit_control_t* bit_control);
    
    inline void bit_toggle(bit_control_t* bit_control);
    
    inline bool bit_read(bit_control_t* bit_control);

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

