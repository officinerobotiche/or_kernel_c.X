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

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "or_peripherals/GPIO/gpio.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/**
 * @brief Get type GPIO from mask
 * @param conf the register to read
 * @param mask the mask
 * @return type of GPIO
 */
inline gpio_type_t gpio_get_type_from_mask(REGISTER conf, int mask) {
    if(REGISTER_MASK_READ(conf, mask)) {
        return GPIO_INPUT;
    } else {
        return GPIO_OUTPUT;
    }
}

inline void gpio_init_pin(gpio_t *pin) {
    switch(pin->type) {
        case GPIO_INPUT:
            REGISTER_MASK_SET_HIGH(pin->CS_TRIS, pin->CS_mask);
            break;
        case GPIO_OUTPUT:
            REGISTER_MASK_SET_LOW(pin->CS_TRIS, pin->CS_mask);
            break;
    }
}

void gpio_init_port(gpio_t *port, size_t size) {
    unsigned int i;
    for(i = 0; i < size; ++i) {
        // Register pin
        gpio_init_pin(&port[i]);
    }
}

inline void gpio_config_pin(gpio_t *pin, gpio_type_t type) {
    pin->type = type;
    gpio_init_pin(pin);
}

void gpio_config_port(gpio_t *port, REGISTER conf, size_t size) {
    unsigned int i;
    for(i = 0; i < size; ++i) {
        // Get type configuration
        gpio_type_t type = gpio_get_type_from_mask(conf, BIT_MASK(i));
        // Register pin
        gpio_config_pin(&port[i], type);
    }
}

/**
 * @brief Get state GPIO from mask
 * @param conf the register to read
 * @param mask the mask
 * @return state of GPIO
 */
inline gpio_state_t gpio_get_state_from_mask(REGISTER conf, int mask) {
    if(REGISTER_MASK_READ(conf, mask)) {
        return GPIO_HIGH;
    } else {
        return GPIO_LOW;
    }
}

inline void gpio_set_pin(gpio_t *port, gpio_state_t value) {
    switch(value) {
        case GPIO_HIGH:
            REGISTER_MASK_SET_HIGH(port->CS_LAT, port->CS_mask);
            break;
        case GPIO_LOW:
            REGISTER_MASK_SET_LOW(port->CS_LAT, port->CS_mask);
            break;
    }
}

void gpio_set_port(gpio_t *port, int value, size_t size) {
    unsigned int i;
    for(i = 0; i < size; ++i) {
        // Get pin value from variable
        gpio_state_t state = gpio_get_state_from_mask((REGISTER) &value, BIT_MASK(i));
        // Set pin value if only the port is an OUTPUT
        if(port->type == GPIO_OUTPUT) {
            gpio_set_pin(&port[i], state);
        }
    }
}

inline int gpio_get_pin(gpio_t *port) {
    switch(port->type) {
        case GPIO_INPUT:
            return gpio_get_state_from_mask(port->CS_PORT, port->CS_mask);
            break;
        case GPIO_OUTPUT:
            return gpio_get_state_from_mask(port->CS_LAT, port->CS_mask);
            break;
        default:
            return 0;
            break;
    }
}

int gpio_get_port(gpio_t *port, size_t size) {
    // Initialize output
    int value = 0;
    unsigned int i;
    for(i = 0; i < size; ++i) {
        if(gpio_get_pin(&port[i]) == GPIO_HIGH) {
            value += BIT_MASK(i);
        }
    }
    return value;
}