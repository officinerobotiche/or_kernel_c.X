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

REGISTER ANALOG;
gp_peripheral_t* GPIO_PORTS;
size_t LEN;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void gpio_init(REGISTER analog, gp_peripheral_t* gpio, size_t len) {
    GPIO_PORTS = gpio;
    ANALOG = analog;
    LEN = len;
    int i;
    for(i = 0; i < LEN; ++i) {
        gpio_register_peripheral(&GPIO_PORTS[i]);
    }
}

void gpio_register(gpio_t* port) {
    switch(port->type) {
        case GPIO_INPUT:
            REGISTER_MASK_SET_HIGH(port->CS_TRIS, port->CS_mask);
            break;
        case GPIO_OUTPUT:
            REGISTER_MASK_SET_LOW(port->CS_TRIS, port->CS_mask);
            break;
        default:
            break;
    }
}

void gpio_register_peripheral(gp_peripheral_t* port) {
    switch(port->gpio.type) {
        case GPIO_INPUT:
            if(port->common.analog != GPIO_NO_PERIPHERAL) {
                REGISTER_MASK_SET_HIGH(ANALOG, BIT_MASK(port->common.analog->number));
            }
            REGISTER_MASK_SET_HIGH(port->gpio.CS_TRIS, port->gpio.CS_mask);
            break;
        case GPIO_OUTPUT:
            if(port->common.analog != GPIO_NO_PERIPHERAL) {
                REGISTER_MASK_SET_HIGH(ANALOG, BIT_MASK(port->common.analog->number));
            }
            REGISTER_MASK_SET_LOW(port->gpio.CS_TRIS, port->gpio.CS_mask);
            break;
        case GPIO_ANALOG:
            REGISTER_MASK_SET_HIGH(port->gpio.CS_TRIS, port->gpio.CS_mask);
            // Set analog the device
            if(port->common.analog != GPIO_NO_PERIPHERAL) {
                REGISTER_MASK_SET_LOW(ANALOG, BIT_MASK(port->common.analog->number));
            }
            break;
    }
}

int gpio_get_analog(int gpioIdx) {
    if(GPIO_PORTS[gpioIdx].gpio.type == GPIO_ANALOG) {
        return 1;
    } else
        return 0;
}

gpio_port_t gpio_get(void) {
    gpio_port_t port = 0;
    int i;
    for (i = 0; i < LEN; ++i) {
        switch (GPIO_PORTS[i].gpio.type) {
            case GPIO_INPUT:
                if(REGISTER_MASK_READ(GPIO_PORTS[i].gpio.CS_PORT, GPIO_PORTS[i].gpio.CS_mask))
                    port += BIT_MASK(i);
                break;
            case GPIO_OUTPUT:
                if(REGISTER_MASK_READ(GPIO_PORTS[i].gpio.CS_LAT, GPIO_PORTS[i].gpio.CS_mask))
                    port += BIT_MASK(i);
                break;
            default:
                break;
        }
    }
    return port;
}

void gpio_set(gpio_port_t port) {
    int i;
    for(i = 0; i < LEN; ++i) {
        if(GPIO_PORTS[i].gpio.type == GPIO_OUTPUT) {
            if(REGISTER_MASK_READ(&port, BIT_MASK(i))) {
                REGISTER_MASK_SET_HIGH(GPIO_PORTS[i].gpio.CS_LAT, GPIO_PORTS[i].gpio.CS_mask);
            } else {
                REGISTER_MASK_SET_LOW(GPIO_PORTS[i].gpio.CS_LAT, GPIO_PORTS[i].gpio.CS_mask);
            }
        }
    }
}