/*
 * Copyright (C) 2014 Officine Robotiche
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

#ifndef EVENTS_H
#define	EVENTS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <string.h>
    
    #include <system/gpio.h>

    #define INVALID_HANDLE 0xFFFF

typedef enum _eventP {
        EVENT_PRIORITY_LOW = 0,
        EVENT_PRIORITY_MEDIUM,
        EVENT_PRIORITY_HIGH,
} eventPriority;

typedef int16_t time_t;

typedef uint16_t hEvent_t;

typedef void (*event_callback_t)(int argc, char *argv);

//typedef void (*event_callback_t)(void);

void init_events(REGISTER timer_register);

void register_interrupt(eventPriority priority, hardware_bit_t* pin);

void trigger_event(hEvent_t hEvent);

void trigger_event_data(hEvent_t hEvent, int argc, char *argv);

hEvent_t register_event(event_callback_t event_callback);

hEvent_t register_event_p(event_callback_t event_callback, eventPriority priority);

inline void event_manager(eventPriority priority);

inline time_t get_time(hEvent_t hEvent);

#ifdef	__cplusplus
}
#endif

#endif	/* EVENTS_H */

