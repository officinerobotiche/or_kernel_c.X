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

#ifndef TASK_MANAGER_H
#define	TASK_MANAGER_H

#ifdef	__cplusplus
extern "C" {
#endif

    #include "system/events.h"

    typedef uint16_t hTask_t;
    
    typedef uint16_t frequency_t;
    
    typedef enum _task_status {
        STOP,
        RUN,
    } task_status_t;
    
    typedef struct _task {
        hTask_t task;
        frequency_t frequency;
    } task_t;
    
    void task_init(void);
    
    hTask_t task_load(hEvent_t hEvent, frequency_t frequency);
    
    hTask_t task_load_data(hEvent_t hEvent, frequency_t frequency, int argc, char argv);
    
    bool task_status(hTask_t hTask, task_status_t run);
    
    bool change_frequency(hTask_t hTask, frequency_t frequency);
    
    bool task_unload(hTask_t hTask);
    
    string_data_t get_task_name(hTask_t taskIndex);
    
    unsigned short get_task_number(void);

    inline void task_manager(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TASK_MANAGER_H */

