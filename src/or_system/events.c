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

#include <xc.h>

#include "or_system/events.h"
#include "or_peripherals/GPIO/gpio.h"

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
    hModule_t name;
} EVENT;
/**
 * Information about hardware interrupt
 * Bit Interrupt to call
 * state of interrupt
 */
typedef struct _interrupt_bit {
    hardware_bit_t* interrupt_bit;
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
    events[eventIndex].priority = EVENT_PRIORITY_LOW;
    events[eventIndex].overTmr = 0;
    events[eventIndex].time = 0;
    events[eventIndex].argc = 0;
    events[eventIndex].argv = NULL;
    events[eventIndex].name = NULL;
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

void register_interrupt(eventPriority priority, hardware_bit_t* pin) {
    interrupts[priority].interrupt_bit = pin;
    REGISTER_MASK_SET_LOW(interrupts[priority].interrupt_bit->REG, interrupts[priority].interrupt_bit->CS_mask);
    interrupts[priority].available = true;
    event_counter++;
}

void trigger_event(hEvent_t hEvent) {
    trigger_event_data(hEvent, 0, NULL);
}

void trigger_event_data(hEvent_t hEvent, int argc, int *argv) {
    if (hEvent < MAX_EVENTS) {
        if (events[hEvent].event_callback != NULL) {
            events[hEvent].eventPending = TRUE;
            events[hEvent].argc = argc;
            events[hEvent].argv = argv;
            REGISTER_MASK_SET_HIGH(interrupts[events[hEvent].priority].interrupt_bit->REG, interrupts[events[hEvent].priority].interrupt_bit->CS_mask);
        }
    }
}

hEvent_t register_event(hModule_t name, event_callback_t event_callback) {
    return register_event_p(name, event_callback, EVENT_PRIORITY_MEDIUM);
}

hEvent_t register_event_p(hModule_t name, event_callback_t event_callback, eventPriority priority) {
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

hModule_t get_event_name(hEvent_t eventIndex) {
    return events[eventIndex].name;
}

inline void event_manager(eventPriority priority) {
    int save_to;
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
                    SET_AND_SAVE_CPU_IPL(save_to, LEVEL);
                    pEvent->event_callback(pEvent->argc, pEvent->argv);             ///< Launch callback
                    pEvent->eventPending = FALSE;                                   ///< Complete event
                    RESTORE_CPU_IPL(save_to);
                    // Time of execution
                    if(pEvent->overTmr == 0) {                  
                        pEvent->time = (*timer) - time;
                    } else {
                        pEvent->time = ((*timer) + (0xFFFF - time)
                                + (0xFFFF * (pEvent->overTmr - 1)));
                    }
                }
            } else if(pEvent->eventPending == WORKING) {
                pEvent->overTmr++;
            }
        }
    }
}

inline uint32_t get_time(hEvent_t hEvent) {
    if (hEvent != INVALID_EVENT_HANDLE) {
        return  events[hEvent].time*time_sys;
    } else return 0;
}