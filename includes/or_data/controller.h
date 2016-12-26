/*
 * Copyright (C) 2015 Officine Robotiche
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

#ifndef DATA_H
#define	DATA_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>    
#include "or_peripherals/GPIO/gpio.h"

/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
    /**
     * Definition of string, with poin pointer to string and relative length
     */
    typedef struct _string_data {
        const char* string;
        unsigned int len;
    } string_data_t;
    
    typedef uint16_t data_address_t;
    
    typedef void (*data_controller_cb)(bool status);
    
    typedef bool (*data_func)(void *type, data_address_t address, int *buff, size_t size, data_controller_cb cb);
    
    typedef struct {
        data_func writer;
        data_func reader;
        void *obj;
    } DATA_controller_t;
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
    /**
     * Protected memcpy, stop particular interrupt and copy data.
     * @param reg Interrupt to disable
     * @param destination data
     * @param source data
     * @param num size of data
     */
    void protectedMemcpy(hardware_bit_t* reg, void *destination, const void *source, size_t num);

    
void DATA_controller_init(DATA_controller_t *controller, void* obj, data_func writer, data_func reader);

#ifdef	__cplusplus
}
#endif

#endif	/* DATA_H */

