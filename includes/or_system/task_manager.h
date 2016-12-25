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

#ifndef TASK_MANAGER_H
#define	TASK_MANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include <stdarg.h>
    #include "or_system/events.h"

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    /// Invalid handle for event
    #define INVALID_TASK_HANDLE 0xFFFF
    /// Invalid handle for event
    #define INVALID_FREQUENCY 0xFFFFFFFF

    /// Definition of Task
    typedef uint16_t hTask_t;
    /// time function in [uS]
    typedef int16_t time_t;
    /**
     * Definition status task:
     * STOP - The task is loaded, but does not work
     * RUN  - The task is loaded and working
     */
    typedef enum _task_status {
        STOP,
        RUN,
    } task_status_t;
    /**
     * Structure definition of task:
     * * task number
     * * frequency of this task
     */
    typedef struct _task {
        hTask_t task;
        frequency_t frequency;
    } task_t;
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
    /**
     * Initialization task manager
     * @param timer_frequency frequency timer
     */
    void task_init(frequency_t timer_frequency);
    /**
     * Load event in task manager, with a frequency and arguments to lanch when started
     * @param hEvent number event
     * @param frequency frequency to automatic start
     * @return number task
     */
    hTask_t task_load(hEvent_t hEvent, frequency_t frequency);
    /**
     * Load event in task manager, with a frequency and arguments to lanch when started
     * @param hEvent number event
     * @param frequency frequency to automatic start
     * @param argc number arguments
     * @param argv arguments
     * @return number task
     */
    hTask_t task_load_data(hEvent_t hEvent, frequency_t frequency, int argc, ...);
    /**
     * Set tast to run or stop
     * @param hTask number task
     * @param run RUN or STOP
     * @return if task is correct return true
     */
    bool task_set(hTask_t hTask, task_status_t run);
    /**
     * Change frequency operation 
     * @param hTask number task
     * @param frequency new frequency
     * @return if frequency is minor to timer frequncy return true
     */
    bool task_set_frequency(hTask_t hTask, frequency_t frequency);
    /**
     * Unload task and remove from task avaliable
     * @param hTask Number task
     * @return if task exist
     */
    bool task_unload(hTask_t hTask);
    /**
     * Number of registered tasks
     * @return number task
     */
    unsigned short get_task_number(void);
    /**
     *  This function you must call in timer function
     */
    inline void task_manager(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TASK_MANAGER_H */

