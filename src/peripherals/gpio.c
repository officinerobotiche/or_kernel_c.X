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

gpio_t* GPIO_PORTS;
size_t LEN;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void gpio_init(REGISTER analog, gpio_t* gpio, size_t len) {
    GPIO_PORTS = gpio;
    LEN = len;
    int i;
    for(i = 0; i < LEN; ++i) {
        gpio_register(&GPIO_PORTS[i]);
    }
}

void gpio_register(gpio_t* port) {
    gpio_register_analog(port, -1);
}

void gpio_register_analog(gpio_t* port, int mask_analog) {
    switch(port->type) {
        case GPIO_INPUT:
            REGISTER_MASK_SET_HIGH(port->CS_TRIS, port->CS_mask);
            break;
        case GPIO_OUTPUT:
            REGISTER_MASK_SET_LOW(port->CS_TRIS, port->CS_mask);
            break;
        case GPIO_ANALOG:
            break;
    }
}

GPIO_PORT_T gpio_get(void) {
    GPIO_PORT_T port = 0;
    int i;
    for (i = 0; i < LEN; ++i) {
        switch (GPIO_PORTS[i].type) {
            case GPIO_INPUT:
                if(REGISTER_MASK_READ(GPIO_PORTS[i].CS_PORT, GPIO_PORTS[i].CS_mask))
                    port += BIT_MASK(i);
                break;
            case GPIO_OUTPUT:
                if(REGISTER_MASK_READ(GPIO_PORTS[i].CS_LAT, GPIO_PORTS[i].CS_mask))
                    port += BIT_MASK(i);
                break;
            default:
                break;
        }
    }
    return port;
}

void gpio_set(GPIO_PORT_T port) {
    int i;
    for(i = 0; i < LEN; ++i) {
        if(GPIO_PORTS[i].type == GPIO_OUTPUT) {
            if(REGISTER_MASK_READ(&port, BIT_MASK(i))) {
                REGISTER_MASK_SET_HIGH(GPIO_PORTS[i].CS_LAT, GPIO_PORTS[i].CS_mask);
            } else {
                REGISTER_MASK_SET_LOW(GPIO_PORTS[i].CS_LAT, GPIO_PORTS[i].CS_mask);
            }
        }
    }
}