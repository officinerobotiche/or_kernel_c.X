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

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system/events.h"
#include "system/gpio.h"

#define LNG_EVENTPRIORITY sizeof(eventPriority)

#define MAX_EVENTS 16

typedef struct _tagEVENT {
        bool eventPending;
        event_callback_t event_callback;
        uint8_t priority;
        time_t time;
} EVENT;

typedef struct _interrupt_bit {
    bit_control_t interrupt_bit;
    bool available;
} interrupt_bit_t;

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

interrupt_bit_t interrupts[LNG_EVENTPRIORITY];
EVENT events[MAX_EVENTS];
bool event_init_done = false;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void init_events(void) {
    hEvent_t eventIndex;
    unsigned short priorityIndex;
    
    for (eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++) {
        events[eventIndex].event_callback = NULL;
        events[eventIndex].eventPending = false;
        events[eventIndex].priority = EVENT_PRIORITY_LOW;
        events[eventIndex].time = 0;
    }
    for (priorityIndex = 0; priorityIndex < LNG_EVENTPRIORITY; ++priorityIndex) {
        interrupts[priorityIndex].available = false;
    }
    event_init_done = true;
}

void register_interrupt(eventPriority priority, hardware_bit_t* pin) {
    interrupts[priority].interrupt_bit.pin = pin;
    bit_setup(&interrupts[priority].interrupt_bit);
    bit_low(&interrupts[priority].interrupt_bit);
    interrupts[priority].available = true;
}

void trigger_event(hEvent_t hEvent) {
    if (hEvent < MAX_EVENTS) {
        if (events[hEvent].event_callback != NULL) {
            events[hEvent].eventPending = true;
            bit_high(&interrupts[events[hEvent].priority].interrupt_bit);
        }
    }
}

hEvent_t register_event(event_callback_t event_callback) {
    return register_event_p(event_callback, EVENT_PRIORITY_MEDIUM);
}

hEvent_t register_event_p(event_callback_t event_callback, eventPriority priority) {
    hEvent_t eventIndex;

    if (interrupts[priority].available) {
        for (eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++) {
            if (events[eventIndex].event_callback == NULL) {
                events[eventIndex].event_callback = event_callback;
                events[eventIndex].priority = priority;
                return eventIndex;
            }
        }
    }
    return INVALID_HANDLE;
}

inline void event_manager(eventPriority priority) {
    hEvent_t eventIndex;
    EVENT* pEvent;

    if (event_init_done) {
        for (eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++) {
            pEvent = &events[eventIndex];
            if ((pEvent->eventPending == true) && (pEvent->priority == priority)) {
                pEvent->eventPending = false;
                if (pEvent->event_callback != NULL) {
                    pEvent->time = pEvent->event_callback();
                }
            }
        }
    }
}

inline time_t get_time(hEvent_t hEvent) {
    return events[hEvent].time;
}