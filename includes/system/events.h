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

typedef enum _eventP {
        EVENT_PRIORITY_LOW = 0,
        EVENT_PRIORITY_MEDIUM,
        EVENT_PRIORITY_HIGH,
} eventPriority;

typedef void (*event_callback_t)(void);

typedef int16_t frequency_t;

typedef uint16_t hEvent_t;

void init_events(void);

void trigger_event(hEvent_t hEvent);

uint16_t register_event(event_callback_t event_callback);

uint16_t register_event_p(event_callback_t event_callback, eventPriority priority, frequency_t frequency);

inline void event_manager(eventPriority priority);

#ifdef	__cplusplus
}
#endif

#endif	/* EVENTS_H */

