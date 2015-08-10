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

#include <stdarg.h>
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
    /// Build a Max bit in x position
    #define BIT_MASK(x)                       (1 << (x))
    #define GPIO_NO_PERIPHERAL NULL
    #define GPIO_ANALOG_CONF(array, an) \
                (array).number = (an);  \
                (array).value = 0;

    //Rule of thumb: Always read inputs from PORTx and write outputs to LATx. 
    //If you need to read what you set an output to, read LATx.
    /// Port builder
    //#define GPIO_INIT(x, n, type)             {&(TRIS##x), &(PORT##x), &(LAT##x), BIT_MASK(n), (type)}
    #define GPIO_INIT_TYPE(array, x, n, type_n)  \
                (array).CS_TRIS = &(TRIS##x);    \
                (array).CS_PORT = &(PORT##x);    \
                (array).CS_LAT = &(LAT##x);      \
                (array).CS_mask = BIT_MASK((n)); \
                (array).type = (type_n);
    /// Simple initialization a GPIO
    #define GPIO_INIT(array, x, n)  GPIO_INIT_TYPE((array).gpio, x, n, GPIO_INPUT)  \
                                        (array).common.analog = GPIO_NO_PERIPHERAL;
    /// Initialization with analog
    #define GPIO_INIT_ANALOG(array, x, n, adc) GPIO_INIT_TYPE((array).gpio, x, n, GPIO_INPUT) \
                                                (array).common.analog = (gp_analog_t*) adc;
    /// GPIO port builder
    #define GPIO_PORT_INIT(port, GP, SIZE)      \
                (port).gpio = (GP);             \
                (port).len = (SIZE);
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
    /// Callback to configure the ADC
    typedef bool (*gpio_adc_callbackFunc_t)(void);
    /**
     * 
     */
    typedef enum {
        GPIO_INPUT,
        GPIO_OUTPUT,
        GPIO_ANALOG
    } gpio_type_t;
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
    /**
     * 
     */
    typedef short gpio_name_t;
    /**
     * 
     */
    typedef int16_t gpio_port_t;
    /**
     * 
     */
    typedef struct _gpio {
        REGISTER CS_TRIS;
        REGISTER CS_PORT;
        REGISTER CS_LAT;
        unsigned int CS_mask;
        gpio_type_t type;
    } gpio_t;
    /**
     * 
     */
    typedef struct _gp_analog {
        unsigned short number;
        int value;
    } gp_analog_t;
    /**
     * 
     */
    typedef struct _gp_common {
        gp_analog_t* analog;
        hardware_bit_t* generic;
    } gp_common_t;
    /**
     * 
     */
    typedef struct _gp_peripheral {
        gpio_t gpio;
        gp_common_t common;
    } gp_peripheral_t;
    /**
     * 
     */
    typedef struct _gp_port_def {
        gp_peripheral_t* gpio;
        size_t len;
    } gp_port_def_t;
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
    
    /**
     * 
     * @param gpio
     * @param len
     */
    bool gpio_init(REGISTER analog, gpio_adc_callbackFunc_t call, int len, ...);
    /**
     * 
     * @param port
     */
    void gpio_register(gpio_t* port);
    /**
     * 
     * @param port
     */
    bool gpio_register_peripheral(gp_peripheral_t* port);
    /**
     * 
     * @param port
     * @param type
     */
    void gpio_setup(gpio_name_t name, gpio_port_t port, gpio_type_t type);
    /**
     * 
     * @param gpioIdx
     * @return 
     */
    int gpio_get_analog(gpio_name_t name, short gpioIdx);
    /**
     * 
     * @return 
     */
    gpio_port_t gpio_get(gpio_name_t name);
    
    /**
     * 
     * @param port
     * @return 
     */
    void gpio_set(gpio_name_t name, gpio_port_t port);
    
    /**
     * 
     * @param idx
     * @param AdcBuffer
     * @param len
     */
    inline void gpio_ProcessADCSamples(short idx, unsigned int* AdcBuffer, size_t len) ;

#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

