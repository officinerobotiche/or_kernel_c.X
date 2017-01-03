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

/*****************************************************************************/
/* Files to Include                                                          */
/*****************************************************************************/

#include "or_data/controller.h"

/******************************************************************************/
/* Definition                                                                 */
/******************************************************************************/

#define Lo( X )				( unsigned char )( X & 0x00FF )
#define Hi( X )				( unsigned char )( ( X >> 8 ) & 0x00FF )

#define LNG_DATA_HEADER 3

void DATA_controller_write_cb(void *obj, int status);

/******************************************************************************/
/* Variable Declaration                                                       */
/******************************************************************************/

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

void DATA_controller_init(DATA_controller_t *controller, void* obj, 
        data_func writer, data_func reader, char *buff, unsigned int num_cp, unsigned int size_mem) {
    controller->writer = writer;
    controller->reader = reader;
    controller->obj = obj;
    // Reference buffer address
    controller->buff = buff;
    // Setup number copies and size memory
    controller->number_copy = num_cp;
    controller->size_storing = size_mem;
}

void DATA_controller_blocking_cb(void *obj, int status) {
    DATA_controller_t *controller = (DATA_controller_t*) obj;
    controller->block_wait = false;
}

inline void DATA_controller_builder_buffer(DATA_controller_t *controller, char *buff, size_t size) {
    unsigned int i;
    uint16_t cks = 0;
    // Add length structure
    controller->buff[0] = size;
    // Copy the data on write buffer
    memcpy(&controller->buff[1], buff, size);
    // evaluate the checksum
    for(i = 0; i < size; ++i) {
        cks += buff[i];
    }
    // Add the checksum in the last line
    controller->buff[size+1] = Hi(cks);
    controller->buff[size+2] = Lo(cks);
}

inline DATA_status_t DATA_controller_check_buffer(DATA_controller_t *controller) {
    // Check if the length is the same
    unsigned int size = controller->length - LNG_DATA_HEADER;
    if(controller->buff[0] != size) {
        return DATA_ERROR_LENGTH;
    }
    unsigned int i;
    // Evaluate the checksum
    unsigned int cks = 0;
    for(i = 0; i < size; ++i) {
        cks += controller->buff[i+1];
    }
    uint16_t cks_read = (controller->buff[size+1] << 8) + Lo(controller->buff[size+2]);
    if(cks != cks_read) {
        return DATA_ERROR_CHECKSUM;
    }
    return DATA_DONE;
}

void DATA_controller_check_cb(void *obj, int status) {
    DATA_controller_t *controller = (DATA_controller_t*) obj;
    if(status) {
        // Write on storing function the buffer with all data
        data_address_t page = (controller->address << 7) + (controller->counter << 12);
        // Run read controller
        controller->reader(controller->obj, page, controller->buff,
                controller->length, &DATA_controller_write_cb, controller);
    }
}

void DATA_controller_write_cb(void *obj, int status) {
    DATA_controller_t *controller = (DATA_controller_t*) obj;
    if(status) {
        if(DATA_controller_check_buffer(controller) == DATA_DONE) {
            controller->counter_write++;
        }
        if (controller->counter == controller->number_copy - 1) {
            // Complete write and launch the callback
            controller->cb(controller->obj_recall, controller->counter_write);
        } else {
            // increase the counter
            controller->counter++;
            size_t size = controller->length - LNG_DATA_HEADER;
            // Build a data to write
            DATA_controller_builder_buffer(controller, controller->start_buff, size);
            // Write on storing function the buffer with all data
            data_address_t page = (controller->address << 7) + (controller->counter << 12);
            // Write in new area
            controller->writer(controller->obj, page, controller->buff,
                    controller->length, &DATA_controller_check_cb, controller);
        }
    }
}

bool DATA_controller_write(DATA_controller_t *controller, data_address_t address, char *buff, size_t size, data_controller_cb cb, void *obj) {
    // Save the callback and object
    if(cb == NULL) {
        controller->cb = &DATA_controller_blocking_cb;
        controller->obj_recall = controller;
        controller->block_wait = true;
    } else {
        controller->cb = cb;
        controller->obj_recall = obj;
    }
    // Save the address
    controller->address = address;
    // reset counter
    controller->counter = 0;
    // The page to write the data
    data_address_t page = (controller->address << 7) + (controller->counter << 12);
    // reset counter good write
    controller->counter_write = 0;
    // Save pointer read buffer
    controller->start_buff = buff;
    controller->length = size + LNG_DATA_HEADER;
    // Build a data to write
    DATA_controller_builder_buffer(controller, controller->start_buff, size);
    // Write data
    controller->writer(controller->obj, page, controller->buff, 
                controller->length, &DATA_controller_check_cb, controller);
    // Blocking function
    if(cb == NULL) {
        // Wait the complete read
        while(controller->block_wait);
    }
    return true;
}

bool DATA_controller_blocking_write(DATA_controller_t *controller, data_address_t address, char *buff, size_t size) {
    return DATA_controller_write(controller, address, buff, size, NULL, NULL);
}

void DATA_controller_read_cb(void *obj, int status) {
    DATA_controller_t *controller = (DATA_controller_t*) obj;
    if(status) {
        if(DATA_controller_check_buffer(controller) == DATA_DONE) {
            // Copy the data on read buffer
            memcpy(controller->start_buff, &controller->buff[1], controller->buff[0]);
            // Complete read and launch the callback
            controller->cb(controller->obj_recall, true);
        } else {
            if(controller->counter == controller->number_copy) {
                // Complete read and launch the callback
                controller->cb(controller->obj_recall, false);
            } else {
                // increase the counter
                controller->counter++;
                // Write on storing function the buffer with all data
                data_address_t page = (controller->address << 7) + (controller->counter << 12);
                // Run read controller
                controller->reader(controller->obj, page, controller->buff,
                        controller->length, &DATA_controller_read_cb, controller);
            }
        }
    } else {
        // launch the callback with error
        controller->cb(controller->obj_recall, false);
    }
}

bool DATA_controller_read(DATA_controller_t *controller, data_address_t address, char *buff, size_t size, data_controller_cb cb, void *obj) {
    // Save the callback and object
    if(cb == NULL) {
        controller->cb = &DATA_controller_blocking_cb;
        controller->obj_recall = controller;
        controller->block_wait = true;
    } else {
        controller->cb = cb;
        controller->obj_recall = obj;
    }
    // Save the address
    controller->address = address;
    // reset counter
    controller->counter = 0;
    // The page to read the data
    data_address_t page = (controller->address << 7) + (controller->counter << 12);
    // Save pointer read buffer
    controller->start_buff = buff;
    // Set length to read
    controller->length = size + LNG_DATA_HEADER;
    // Run read controller
    controller->reader(controller->obj, page, controller->buff, 
                controller->length, &DATA_controller_read_cb, controller);
    // Blocking function
    if(cb == NULL) {
        // Wait the complete read
        while(controller->block_wait);
    }
    return true;
}

bool DATA_controller_blocking_read(DATA_controller_t *controller, data_address_t address, char *buff, size_t size) {
    return DATA_controller_read(controller, address, buff, size, NULL, NULL);
}