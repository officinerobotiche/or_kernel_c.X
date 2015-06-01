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
#include "peripherals/gpio.h"

#define LNG_EVENTPRIORITY sizeof(eventPriority)

#define MAX_EVENTS 16

typedef enum _event_type {
    FALSE = 0,
    WORKING,
    TRUE,
} EVENT_TYPE;

typedef struct _tagEVENT {
        EVENT_TYPE eventPending;
        event_callback_t event_callback;
        uint16_t overTmr;
        int argc;
        char* argv;
        uint8_t priority;
        time_t time;
        string_data_t* name;
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
unsigned short event_counter = 0;
REGISTER timer;
REGISTER PRTIMER;

/*****************************************************************************/
/* Communication Functions                                                   */

/*****************************************************************************/

void reset_event(hEvent_t eventIndex) {
    events[eventIndex].event_callback = NULL;
    events[eventIndex].eventPending = FALSE;
    events[eventIndex].priority = EVENT_PRIORITY_LOW;
    events[eventIndex].overTmr = 0;
    events[eventIndex].time = 0;
    events[eventIndex].argc = 0;
    events[eventIndex].argv = NULL;
    events[eventIndex].name = NULL;
}

void init_events(REGISTER timer_register, REGISTER pr_timer) {
    hEvent_t eventIndex;
    unsigned short priorityIndex;
    timer = timer_register;
    PRTIMER = pr_timer;
    for (eventIndex = 0; eventIndex < MAX_EVENTS; ++eventIndex) {
        reset_event(eventIndex);
    }
    for (priorityIndex = 0; priorityIndex < LNG_EVENTPRIORITY; ++priorityIndex) {
        interrupts[priorityIndex].available = false;
    }
    event_counter = 0;
}

void register_interrupt(eventPriority priority, hardware_bit_t* pin) {
    interrupts[priority].interrupt_bit.pin = pin;
    bit_setup(&interrupts[priority].interrupt_bit);
    bit_low(&interrupts[priority].interrupt_bit);
    interrupts[priority].available = true;
    event_counter++;
}

void trigger_event(hEvent_t hEvent) {
    trigger_event_data(hEvent, 0, NULL);
}

void trigger_event_data(hEvent_t hEvent, int argc, char *argv) {
    if (hEvent < MAX_EVENTS) {
        if (events[hEvent].event_callback != NULL) {
            events[hEvent].eventPending = TRUE;
            events[hEvent].argc = argc;
            events[hEvent].argv = argv;
            bit_high(&interrupts[events[hEvent].priority].interrupt_bit);
        }
    }
}

hEvent_t register_event(event_callback_t event_callback, string_data_t* name) {
    return register_event_p(event_callback, name, EVENT_PRIORITY_MEDIUM);
}

hEvent_t register_event_p(event_callback_t event_callback, string_data_t* name, eventPriority priority) {
    hEvent_t eventIndex;

    if (interrupts[priority].available) {
        for (eventIndex = 0; eventIndex < MAX_EVENTS; ++eventIndex) {
            if (events[eventIndex].event_callback == NULL) {
                events[eventIndex].event_callback = event_callback;
                events[eventIndex].priority = priority;
                events[eventIndex].name = name;
                return eventIndex;
            }
        }
    }
    return INVALID_HANDLE;
}

bool unregister_event(hEvent_t eventIndex) {
    if (event_counter <= 0 && eventIndex != INVALID_HANDLE) {
        reset_event(eventIndex);
        event_counter--;
        return true;
    } else
        return false;
}

string_data_t get_event_name(hEvent_t eventIndex) {
    return *events[eventIndex].name;
}

inline void event_manager(eventPriority priority) {

    if (event_counter > 0) {
        hEvent_t eventIndex;
        EVENT* pEvent;
        for (eventIndex = 0; eventIndex < MAX_EVENTS; ++eventIndex) {
            pEvent = &events[eventIndex];
            if ((pEvent->eventPending == TRUE) && (pEvent->priority == priority)) {
                if (pEvent->event_callback != NULL) {
                    volatile time_t time;
                    pEvent->eventPending = WORKING;
                    pEvent->overTmr = 0;                                            ///< Reset timer
                    time = *timer;                                                  ///< Timing function
                    pEvent->event_callback(pEvent->argc, pEvent->argv);             ///< Launch callback
                    pEvent->eventPending = FALSE;                                   ///< Complete event
                    pEvent->time = pEvent->overTmr * (*PRTIMER) + (*timer) - time;  ///< Time of execution
                }
            } else if(pEvent->eventPending == WORKING) {
                pEvent->overTmr++;
            }
        }
    }
}

inline time_t get_time(hEvent_t hEvent) {
    if (hEvent != INVALID_HANDLE) {
        return events[hEvent].time;
    } else return 0;
}