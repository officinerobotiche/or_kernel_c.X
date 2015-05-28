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

#define MAX_EVENTS 16
#define INVALID_HANDLE 0xFFFF

typedef struct _tagEVENT {
        bool eventPending;
        event_callback_t event_callback;
        uint8_t priority;
        frequency_t frequency;
} EVENT;

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

EVENT events[MAX_EVENTS];
bool event_init_done = false;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void init_events(void) {
    hEvent_t eventIndex;

    for (eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++) {
        events[eventIndex].event_callback = NULL;
        events[eventIndex].eventPending = false;
        events[eventIndex].priority = EVENT_PRIORITY_LOW;
        events[eventIndex].frequency = -1;
    }
    event_init_done = true;
}

void trigger_event(hEvent_t hEvent) {
    if (hEvent < MAX_EVENTS) {
        if (events[hEvent].event_callback != NULL) {
            events[hEvent].eventPending = true;
            switch (events[hEvent].priority) {
                case EVENT_PRIORITY_LOW:
                    //_EVENTL_TRIGGERIF = 1; // trigger the interrupt
                    break;
                case EVENT_PRIORITY_MEDIUM:
                    //_EVENTM_TRIGGERIF = 1; // trigger the interrupt
                    break;
                case EVENT_PRIORITY_HIGH:
                    //_EVENTH_TRIGGERIF = 1;  // trigger the interrupt
                    break;
            }
        }
    }
}

hEvent_t register_event(event_callback_t event_callback) {
    return register_event_p(event_callback, EVENT_PRIORITY_MEDIUM, -1);
}

hEvent_t register_event_p(event_callback_t event_callback, eventPriority priority, frequency_t frequency) {
    hEvent_t eventIndex;

    for (eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++) {
        if (events[eventIndex].event_callback == NULL) {
            events[eventIndex].event_callback = event_callback;
            events[eventIndex].priority = priority;
            events[eventIndex].frequency = frequency;
            return eventIndex;
        }
    }
    return INVALID_HANDLE;
}

/* inline */
void event_manager(eventPriority priority) {
    hEvent_t eventIndex;
    EVENT* pEvent;

    if (event_init_done) {
        for (eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++) {
            pEvent = &events[eventIndex];
            if ((pEvent->eventPending == true) && (pEvent->priority == priority)) {
                pEvent->eventPending = false;
                if (pEvent->event_callback != NULL) {
                    pEvent->event_callback();
                }
            }
        }
    }
}
