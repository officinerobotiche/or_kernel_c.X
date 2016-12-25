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

#include "or_system/events.h"

/// Max number of events
#define MAX_EVENTS 16
/**
 * Event state:
 * FALSE: The event doesn't running
 * WORKING: The event is in elaboration
 * TRUE: The event finished
 */
typedef enum _event_type {
    FALSE = 0,
    WORKING,
    TRUE,
} EVENT_TYPE;
/**
 * Information about event:
 * State of event
 * Function to call
 * number of argument
 * arguments
 * priority
 * overflow timer
 * time to computation
 * Name event
 */
typedef struct _tagEVENT {
    EVENT_TYPE eventPending;
    event_callback_t event_callback;
    int argc;
    int* argv;
    eventPriority priority;
    uint16_t overTmr;
    uint32_t time;
} EVENT;
/**
 * Information about hardware interrupt
 * Bit Interrupt to call
 * state of interrupt
 */
typedef struct _interrupt_bit {
    const hardware_bit_t* interrupt_bit;
    bool available;
} interrupt_bit_t;

#define NANO_SEC_MOLTIPLICATOR 1000000000

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
/// Declare an array with all interrupts
interrupt_bit_t interrupts[LNG_EVENTPRIORITY];
/// Declare an array with all events
EVENT events[MAX_EVENTS];
/// Number of all event registered
unsigned short event_counter = 0;
/// Timer register
REGISTER timer;
/// Counter time register
REGISTER PRTIMER;
/// Frequency MicroController Unit (MCU)
unsigned long time_sys;
/// Maskable interrupt level
unsigned int LEVEL;

/******************************************************************************/
/* Communication Functions                                                    */
/******************************************************************************/
/**
 * Reset event, with default configuration
 * @param eventIndex Number of array
 */
void reset_event(hEvent_t eventIndex) {
    events[eventIndex].event_callback = NULL;
    events[eventIndex].eventPending = FALSE;
    events[eventIndex].priority = EVENT_PRIORITY_CPU;
    events[eventIndex].overTmr = 0;
    events[eventIndex].time = 0;
    events[eventIndex].argc = 0;
    events[eventIndex].argv = NULL;
}

void init_events(REGISTER timer_register, REGISTER pr_timer, frequency_t frq_mcu, unsigned int level) {
    hEvent_t eventIndex;
    unsigned short priorityIndex;
    timer = timer_register;
    PRTIMER = pr_timer;
    LEVEL = level;
    time_sys = NANO_SEC_MOLTIPLICATOR/frq_mcu;
    for (eventIndex = 0; eventIndex < MAX_EVENTS; ++eventIndex) {
        reset_event(eventIndex);
    }
    for (priorityIndex = 0; priorityIndex < LNG_EVENTPRIORITY; ++priorityIndex) {
        interrupts[priorityIndex].available = false;
    }
    event_counter = 0;
}

void register_interrupt(eventPriority priority, const interrupt_controller_t* interrupt_controller) {
    interrupts[priority].interrupt_bit = &interrupt_controller->FLAG;
    // Disable the interrupt
    REGISTER_MASK_SET_LOW(interrupt_controller->ENABLE.REG, interrupt_controller->ENABLE.CS_mask);
    // Set priority
    // TODO
    // Set Low the flag
    REGISTER_MASK_SET_LOW(interrupts[priority].interrupt_bit->REG, interrupts[priority].interrupt_bit->CS_mask);
    // Enable the interrupt
    REGISTER_MASK_SET_HIGH(interrupt_controller->ENABLE.REG, interrupt_controller->ENABLE.CS_mask);
    interrupts[priority].available = true;
    event_counter++;
}

inline void trigger_event(hEvent_t hEvent) {
    trigger_event_data(hEvent, 0, NULL);
}

inline void trigger_event_data(hEvent_t hEvent, int argc, int *argv) {
    if (hEvent < MAX_EVENTS) {
        if (events[hEvent].event_callback != NULL) {
            events[hEvent].eventPending = TRUE;
            events[hEvent].argc = argc;
            events[hEvent].argv = argv;
            // Set high the interrupt register.
            if(events[hEvent].priority != EVENT_PRIORITY_CPU) {
                REGISTER_MASK_SET_HIGH(interrupts[events[hEvent].priority].interrupt_bit->REG, interrupts[events[hEvent].priority].interrupt_bit->CS_mask);
            }
        }
    }
}

hEvent_t register_event(event_callback_t event_callback) {
    return register_event_p(event_callback, EVENT_PRIORITY_CPU);
}

hEvent_t register_event_p(event_callback_t event_callback, eventPriority priority) {
    hEvent_t eventIndex;

    if (interrupts[priority].available || priority == EVENT_PRIORITY_CPU) {
        for (eventIndex = 0; eventIndex < MAX_EVENTS; ++eventIndex) {
            if (events[eventIndex].event_callback == NULL) {
                events[eventIndex].event_callback = event_callback;
                events[eventIndex].priority = priority;
                return eventIndex;
            }
        }
    }
    return INVALID_EVENT_HANDLE;
}

bool unregister_event(hEvent_t eventIndex) {
    if (event_counter <= 0 && eventIndex != INVALID_EVENT_HANDLE) {
        reset_event(eventIndex);
        event_counter--;
        return true;
    } else
        return false;
}

inline void event_manager(eventPriority priority) {
    if (event_counter > 0) {
        hEvent_t eventIndex;
        EVENT* pEvent;
        for (eventIndex = 0; eventIndex < MAX_EVENTS; ++eventIndex) {
            pEvent = &events[eventIndex];
            if ((pEvent->eventPending == TRUE) && (pEvent->priority == priority)) {
                if (pEvent->event_callback != NULL) {
                    uint16_t time;
                    pEvent->eventPending = WORKING;
                    pEvent->overTmr = 0;                                            ///< Reset timer
                    time = *timer;                                                  ///< Timing function
                    pEvent->event_callback(pEvent->argc, pEvent->argv);             ///< Launch callback
                    pEvent->eventPending = FALSE;                                   ///< Complete event
                    // Time of execution
                    if(pEvent->overTmr == 0) {
                        pEvent->time = (*timer) - time;
                    } else {
                        pEvent->time = ((*timer) + (0xFFFF - time)
                                + (0xFFFF * (pEvent->overTmr - 1)));
                    }
                }
            } else if (pEvent->eventPending == WORKING) {
                pEvent->overTmr++;
            }
        }
    }
    // Set low the interrupt register.
    if (priority != EVENT_PRIORITY_CPU) {
        REGISTER_MASK_SET_LOW(interrupts[priority].interrupt_bit->REG, interrupts[priority].interrupt_bit->CS_mask);
    }
}

inline uint32_t get_time(hEvent_t hEvent) {
    if (hEvent != INVALID_EVENT_HANDLE) {
        return  events[hEvent].time*time_sys;
    } else return 0;
}

inline void EventsController() {
    while(true) {
        // Controller very low priority event
        // This function works runtime
        event_manager(EVENT_PRIORITY_CPU);
    }
}