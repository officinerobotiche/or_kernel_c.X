/*
 * Copyright (C) 2014-2016 Officine Robotiche
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
    
#include "or_peripherals/GPIO/gpio.h"

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    /// General information about interrupt
    typedef struct _interrupt_controller {
        const hardware_bit_t FLAG;
        const hardware_bit_t ENABLE;
    } interrupt_controller_t;
    
#define EVENT_INTERRUPT_INIT(Flag, n, Enable, x) {REGISTER_INIT(Flag, n), REGISTER_INIT(Enable, x)}

    /// Type of events, from low level to high level
    typedef enum _eventP {
        EVENT_PRIORITY_HIGH = 0,
        EVENT_PRIORITY_MEDIUM,
        EVENT_PRIORITY_LOW,
        EVENT_PRIORITY_VERY_LOW,
        EVENT_PRIORITY_CPU
    } eventPriority;
    /// Dimension event priority
    /// The size of eventPriority enumeration minus CPU priority
    #define LNG_EVENTPRIORITY 4
    /// event register number
    typedef uint16_t hEvent_t;
    /// Invalid handle for event
    #define INVALID_EVENT_HANDLE 0xFFFF
    /// Definition of frequency
    typedef uint32_t frequency_t;
    /// Callback when the function start
    typedef void (*event_callback_t)(int argc, int* argv);
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
    /**
     * Initialization event controller, with this function you can set to default 
     * all register events parameter and setup timer evaluation.
     * @param timer_register Timer register
     * @param pr_timer Register counter
     * @param frq_mcu frequency of Microcontroller
     */
    void init_events(REGISTER timer_register, REGISTER pr_timer, frequency_t frq_mcu, unsigned int level);
    /**
     * You can register an event interrupt, with type of priority and bit interrupt
     * @param priority type of priority
     * @param pin Interrupt bit to will be used to start interrupt
     */
    void register_interrupt(eventPriority priority, const interrupt_controller_t* interrupt_controller);
    /**
     * Launch a particular function event
     * @param hEvent number of event
     */
    void trigger_event(hEvent_t hEvent);
    /**
     * Launch the event with data
     * @param hEvent Number event
     * @param argc number of data
     * @param argv datas
     */
    void trigger_event_data(hEvent_t hEvent, int argc, int *argv);
    /**
     * Register an event with a function to call when the event started.
     * Default priority values is EVENT_PRIORITY_MEDIUM
     * @param name associated number module name
     * @param event_callback function to call
     * @return number event
     */
    hEvent_t register_event(event_callback_t event_callback);
    /**
     * Register an event with priority and a function to call when the event started
     * @param name associated number module name
     * @param event_callback function to call
     * @param priority priority for this event
     * @return number event
     */
    hEvent_t register_event_p(event_callback_t event_callback, eventPriority priority);
    /**
     * Remove from list of events the event
     * @param eventIndex index event
     * @return true if is correct unloaded, false otherwhise
     */
    bool unregister_event(hEvent_t eventIndex);
    /**
     * This function you must call in interrupt function and you can set the relative priority
     * @param priority number of priority
     */
    inline void event_manager(eventPriority priority);
    /**
     * Return time to computation the event
     * @param hEvent number event
     * @return time to computation in [nS]
     */
    inline uint32_t get_time(hEvent_t hEvent);
    /**
     * Controller CPU
     */
    inline void EventsController();

#ifdef	__cplusplus
}
#endif

#endif	/* EVENTS_H */

