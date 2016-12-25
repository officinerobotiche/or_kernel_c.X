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

#include "or_peripherals/GPIO/gpio.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

REGISTER ANALOG;
hardware_bit_t* ANA_ON;
hardware_bit_t* DMA_ON;
//gp_peripheral_t* GPIO_PORTS;
gp_port_def_t* GPIO_PORTS[10];
size_t LEN;
gpio_adc_callbackFunc_t gpio_callback;
int *indirect_reference[10];
unsigned int count_analog_gpio = 0;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

bool gpio_init(hardware_bit_t* analog_on, hardware_bit_t* dma_on, REGISTER analog, gpio_adc_callbackFunc_t call, int argc, ...) {
    ANA_ON = analog_on;
    DMA_ON = dma_on;
    ANALOG = analog;
    REGISTER_MASK_SET_HIGH(ANALOG, 0xFFFF);
    va_list argp;
    gpio_callback = call;
    int counter_port, i;
    bool state = true;
    va_start(argp, argc);
    for(counter_port = 0; counter_port < argc; ++counter_port) {
        GPIO_PORTS[counter_port] = va_arg(argp, gp_port_def_t*);
        for(i = 0; i < GPIO_PORTS[counter_port]->len; ++i) {
            state &= gpio_register_peripheral(&GPIO_PORTS[counter_port]->gpio[i]);
        }
    }
    return state;
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

bool gpio_register_peripheral(gp_peripheral_t* port) {
    switch(port->gpio.type) {
        case GPIO_INPUT:
            if(port->common.analog != GPIO_NO_PERIPHERAL) {
                REGISTER_MASK_SET_HIGH(ANALOG, BIT_MASK(port->common.analog->number));
                indirect_reference[port->common.analog->number] = NULL;
                count_analog_gpio--;
            }
            REGISTER_MASK_SET_HIGH(port->gpio.CS_TRIS, port->gpio.CS_mask);
            break;
        case GPIO_OUTPUT:
            if(port->common.analog != GPIO_NO_PERIPHERAL) {
                REGISTER_MASK_SET_HIGH(ANALOG, BIT_MASK(port->common.analog->number));
                indirect_reference[port->common.analog->number] = NULL;
                count_analog_gpio --;
            }
            REGISTER_MASK_SET_LOW(port->gpio.CS_TRIS, port->gpio.CS_mask);
            break;
        case GPIO_ANALOG:
            REGISTER_MASK_SET_HIGH(port->gpio.CS_TRIS, port->gpio.CS_mask);
            // Set analog the device
            if(port->common.analog != GPIO_NO_PERIPHERAL) {
                REGISTER_MASK_SET_LOW(ANALOG, BIT_MASK(port->common.analog->number));
                indirect_reference[port->common.analog->number] = &port->common.analog->value;
                count_analog_gpio++;
            }
            break;
    }
    return true;
}

bool gpio_setup_pin(gpio_name_t name, short gpioIdx, gpio_type_t type) {
    if(GPIO_PORTS[name]->gpio[gpioIdx].gpio.type != type) {
        GPIO_PORTS[name]->gpio[gpioIdx].gpio.type = type;
        return gpio_register_peripheral(&GPIO_PORTS[name]->gpio[gpioIdx]);
    }
    return false;
}

void gpio_setup(gpio_name_t name, uint16_t port, gpio_type_t type) {
    int i;
    int len = GPIO_PORTS[name]->len;
    bool set = true;
    if(type == GPIO_ANALOG) {
        REGISTER_MASK_SET_LOW(ANA_ON->REG, ANA_ON->CS_mask);
        REGISTER_MASK_SET_LOW(DMA_ON->REG, DMA_ON->CS_mask);
    }
    for (i = 0; i < len; ++i) {
        if(REGISTER_MASK_READ(&port, BIT_MASK(i))) {
            set &= gpio_setup_pin(name, i, type);
        }
    }
    if(set && (type == GPIO_ANALOG)) {
        // RUN ADC initializer
        gpio_callback();
        REGISTER_MASK_SET_HIGH(ANA_ON->REG, ANA_ON->CS_mask);
        REGISTER_MASK_SET_HIGH(DMA_ON->REG, DMA_ON->CS_mask);
    }
}

gpio_type_t gpio_config(gpio_name_t name, short port) {
    return GPIO_PORTS[name]->gpio[port].gpio.type;
}

int gpio_get_analog(gpio_name_t name, short gpioIdx) {
    if(GPIO_PORTS[name]->gpio[gpioIdx].gpio.type == GPIO_ANALOG) {
        return GPIO_PORTS[name]->gpio[gpioIdx].common.analog->value;
    } else
        return 0;
}

gpio_port_t gpio_get(gpio_name_t name) {
    gpio_port_t port;
    int i;
    port.port = 0;
    port.len = GPIO_PORTS[name]->len;
    for (i = 0; i < port.len; ++i) {
        switch (GPIO_PORTS[name]->gpio[i].gpio.type) {
            case GPIO_INPUT:
                if(REGISTER_MASK_READ(GPIO_PORTS[name]->gpio[i].gpio.CS_PORT, GPIO_PORTS[name]->gpio[i].gpio.CS_mask))
                    port.port += BIT_MASK(i);
                break;
            case GPIO_OUTPUT:
                if(REGISTER_MASK_READ(GPIO_PORTS[name]->gpio[i].gpio.CS_LAT, GPIO_PORTS[name]->gpio[i].gpio.CS_mask))
                    port.port += BIT_MASK(i);
                break;
            default:
                break;
        }
    }
    return port;
}

void gpio_set(gpio_name_t name, gpio_port_t port) {
    int i;
    int len = GPIO_PORTS[name]->len;
    for(i = 0; i < len; ++i) {
        if(GPIO_PORTS[name]->gpio[i].gpio.type == GPIO_OUTPUT) {
            if(REGISTER_MASK_READ(&port.port, BIT_MASK(i))) {
                REGISTER_MASK_SET_HIGH(GPIO_PORTS[name]->gpio[i].gpio.CS_LAT, GPIO_PORTS[name]->gpio[i].gpio.CS_mask);
            } else {
                REGISTER_MASK_SET_LOW(GPIO_PORTS[name]->gpio[i].gpio.CS_LAT, GPIO_PORTS[name]->gpio[i].gpio.CS_mask);
            }
        }
    }
}

inline void gpio_ProcessADCSamples(short idx, int value) {
    *(indirect_reference[idx]) = value;
}
