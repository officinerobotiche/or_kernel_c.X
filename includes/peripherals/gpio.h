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

#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdbool.h>         /* For true/false definition                     */

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    /// Port builder
    #define GPIO_DEF(LETTER_PORT) _TRIS##LETTER_PORT; \ _PORT##LETTER_PORT;
    
    /// Build a Max bit in x position
    #define BIT_MASK(x)                       (1 << (x))
    /// Initialize hardware_bit_t with name register and bit mask
    #define REGISTER_INIT(reg, x)             {&(reg), BIT_MASK(x)}
    /// Set high bits in register with selected mask
    #define REGISTER_MASK_SET_HIGH(reg, mask) (*(reg) |= (mask))
    /// Set low bits in register with selected mask
    #define REGISTER_MASK_SET_LOW(reg, mask)  (*(reg) &= ~(mask))
    /// Toggle bits in register with selected mask
    #define REGISTER_MASK_TOGGLE(reg, mask)   (*(reg) ^= (mask))
    /// Read bits in register with selected mask
    #define REGISTER_MASK_READ(reg, mask)     ((*(reg) & (mask)) == (mask))
    
    /// Generic definition for register
    typedef volatile unsigned int * REGISTER;
    /**
     * Hardware bit
     * Register
     * Mask with selected bit
     */
    typedef struct _hardware_bit {
        REGISTER CS_PORT;
        unsigned int CS_mask;
    } hardware_bit_t;
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
    /**
     * Set high value selected hardware bit. Use previous define
     * @param bit_control hardware bit
     */
    inline void bit_high(hardware_bit_t* bit_control);
    /**
     * Set low value selected hardware bit. Use previous define
     * @param bit_control hardware bit
     */
    inline void bit_low(hardware_bit_t* bit_control);
    /**
     * Toggle value selected hardware bit. Use previous define
     * @param bit_control hardware bit
     */
    inline void bit_toggle(hardware_bit_t* bit_control);
    /**
     * Read value selected hardware bit. Use previous define
     * @param bit_control hardware bit
     * @return state of hardware bit
     */
    inline bool bit_read(hardware_bit_t* bit_control);

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

