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

/******************************************************************************/
/*	INCLUDE																	  */
/******************************************************************************/
    
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
    
/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
    typedef enum {
        DATA_ERROR_LENGTH,
        DATA_ERROR_CHECKSUM,
        DATA_DONE
    } DATA_status_t;
    
    typedef uint16_t data_address_t;
    
    typedef void (*data_controller_cb)(void *obj, bool status);
    
    typedef bool (*data_func)(void *type, data_address_t address, int *buff, size_t size, data_controller_cb cb, void *obj);
    
    typedef struct {
        data_func writer;
        data_func reader;
        // The pointer of the structure controller of driver
        void *obj;
        // The pointer of buffer
        int *buff;
        // The size of buffer to write
        size_t length;
        // The return callback
        data_controller_cb cb;
        // the return object
        void *obj_recall;
        
        int *read_buff;
        data_address_t address;
        unsigned int counter;
        unsigned int number_copy;
        size_t size_storing;
        bool status;
        // bool wait blocking function complete
        bool block_wait;
    } DATA_controller_t;
    
/******************************************************************************/
/*	FUNCTIONS							 									  */
/******************************************************************************/
/**
 * 
 * @param controller
 * @param obj
 * @param writer
 * @param reader
 * @param buff
 */
void DATA_controller_init(DATA_controller_t *controller, void* obj, 
        data_func writer, data_func reader, int *buff);
/**
 * 
 * @param controller
 * @param address
 * @param buff
 * @param size
 * @param cb
 * @param obj
 * @return 
 */
bool DATA_controller_write(DATA_controller_t *controller, data_address_t address, 
        int *buff, size_t size, data_controller_cb cb, void *obj);
/**
 * 
 * @param controller
 * @param address
 * @param buff
 * @param size
 * @return 
 */
bool DATA_controller_blocking_write(DATA_controller_t *controller, data_address_t address,
        int *buff, size_t size);
/**
 * 
 * @param controller
 * @param address
 * @param buff
 * @param size
 * @param cb
 * @param obj
 * @return 
 */
bool DATA_controller_read(DATA_controller_t *controller, data_address_t address, 
        int *buff, size_t size, data_controller_cb cb, void *obj);
/**
 * 
 * @param controller
 * @param address
 * @param buff
 * @param size
 * @return 
 */
bool DATA_controller_blocking_read(DATA_controller_t *controller, data_address_t address, 
        int *buff, size_t size);

#ifdef	__cplusplus
}
#endif

#endif	/* DATA_H */

