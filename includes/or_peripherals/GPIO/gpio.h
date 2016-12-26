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

#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/*	INCLUDE																	  */
/******************************************************************************/
    
#include <stdarg.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
    /// Build a Max bit in x position
    #define BIT_MASK(x)                       (1 << (x))
    /// Set high bits in register with selected mask
    #define REGISTER_MASK_SET_HIGH(reg, mask) (*(reg) |= (mask))
    /// Set low bits in register with selected mask
    #define REGISTER_MASK_SET_LOW(reg, mask)  (*(reg) &= ~(mask))
    /// Toggle bits in register with selected mask
    #define REGISTER_MASK_TOGGLE(reg, mask)   (*(reg) ^= (mask))
    /// Read bits in register with selected mask
    #define REGISTER_MASK_READ(reg, mask)     ((*(reg) & (mask)) == (mask))

    /**
     * Configuration GPIO
     */
    typedef enum {
        GPIO_INPUT = 1,
        GPIO_OUTPUT = 0
    } gpio_type_t;
    
    typedef enum {
        GPIO_HIGH = 1,
        GPIO_LOW = 0
    } gpio_state_t;

    /// Generic definition for register
    typedef volatile unsigned int * REGISTER;
    /**
     * Hardware bit
     * - Register
     * - Mask with selected bit
     */
    typedef struct _hardware_bit {
        REGISTER REG;
        unsigned int CS_mask;
    } hardware_bit_t;
    /// Initialize hardware_bit_t with name register and bit mask
    #define REGISTER_INIT(reg, x)             {&(reg), BIT_MASK(x)}
    
    typedef struct _gpio {
        REGISTER CS_TRIS;
        REGISTER CS_PORT;
        REGISTER CS_LAT;
        unsigned int CS_mask;
        gpio_type_t type;
    } gpio_t;
    //Rule of thumb: Always read inputs from PORTx and write outputs to LATx. 
    //If you need to read what you set an output to, read LATx.
    /// Port builder
    #define GPIO_INIT(x, n, type) {&(TRIS##x), &(PORT##x), &(LAT##x), BIT_MASK((n)), type}
    #define GPIO_INIT_OUT(x, n) GPIO_INIT(x, n, GPIO_OUTPUT)
    #define GPIO_INIT_IN(x, n) GPIO_INIT(x, n, GPIO_INPUT)
    
/******************************************************************************/
/*	FUNCTIONS							 									  */
/******************************************************************************/
    
    /**
     * @brief Initialization GPIO pin
     * @param pin the pin to initialize
     */
    inline void gpio_init_pin(gpio_t *pin);
    /**
     * @brief Initialization GPIO port
     * @param port the port to initialize
     * @param size the size of the port
     */
    void gpio_init_port(gpio_t *port, size_t size);
    /**
     * @brief Change configuration of the GPIO pin
     * @param pin the pin to change
     * @param type the new type
     */
    inline void gpio_config_pin(gpio_t *pin, gpio_type_t type);
    /**
     * @brief Change configuration of GPIO port
     * @param port The port to change
     * @param conf the new configuration
     * @param size the size of the port
     */
    void gpio_config_port(gpio_t *port, REGISTER conf, size_t size);
    /**
     * @brief Set the level of the GPIO pin
     * @param port the GPIO pin
     * @param value the new state of the GPIO pin
     */
    inline void gpio_set_pin(gpio_t *port, gpio_state_t value);
    /**
     * @brief set new level GPIO port
     * @param port the GPIO port
     * @param value the new values for the GPIO port
     * @param size the size of the port
     */
    void gpio_set_port(gpio_t *port, int value, size_t size);
    /**
     * @brief get state of the GPIO pin
     * @param port the GPIO pin
     * @return the value of the pin
     */
    inline int gpio_get_pin(gpio_t *port);
    /**
     * @brief get state of the GPIO port
     * @param port the GPIO port
     * @param size the size of the port
     * @return the value of the port
     */
    int gpio_get_port(gpio_t *port, size_t size);

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

