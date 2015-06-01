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

#include "system/task_manager.h"

#define MAX_TASKS 16

typedef struct _tagTASK {
    task_status_t run;
    hEvent_t event;
    uint16_t counter;
    frequency_t frequency;
    int argc;
    char *argv;
} TASK;

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

TASK tasks[MAX_TASKS];
unsigned short task_count = 0;

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void task_init(void) {
    hTask_t taskIndex;
    
    for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
        tasks[taskIndex].run = STOP;
        tasks[taskIndex].counter = 0;
        tasks[taskIndex].frequency = 0;
        tasks[taskIndex].event = INVALID_HANDLE;
        tasks[taskIndex].argc = 0;
        tasks[taskIndex].argv = NULL;
    }
}

hTask_t task_load(hEvent_t hEvent, frequency_t frequency) {
    return task_load_data(hEvent, frequency, 0, NULL);
}

hTask_t task_load_data(hEvent_t hEvent, frequency_t frequency, int argc, char *argv) {
    hTask_t taskIndex;
    
    for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
        if (tasks[taskIndex].event == INVALID_HANDLE) {
            tasks[taskIndex].run = STOP;
            tasks[taskIndex].event = hEvent;
            tasks[taskIndex].frequency = frequency;
            tasks[taskIndex].argc = argc;
            tasks[taskIndex].argv = argv;
            task_count++;
            return taskIndex;
        }
    }
    return INVALID_HANDLE;
}

bool task_status(hTask_t hTask, task_status_t run) {
    if(hTask != INVALID_HANDLE) {
        tasks[hTask].run = run;
        return true;
    }
    return false;
}

bool change_frequency(hTask_t hTask, frequency_t frequency) {
    if(hTask != INVALID_HANDLE) {
        if(frequency > 0) {
            tasks[hTask].frequency = frequency;
            return true;
        } else {
            return task_unload(hTask);
        }
    }
    return false;
}

bool task_unload(hTask_t hTask) {
    if(hTask != INVALID_HANDLE) {
        tasks[hTask].event = INVALID_HANDLE;
        tasks[hTask].run = STOP;
        task_count--;
        return true;
    }
    return false;
}

string_data_t get_task_name(hTask_t taskIndex) {
    return get_event_name(tasks[taskIndex].event);
}

inline void task_manager(void) {
    if(task_count > 0) {
        hTask_t taskIndex;
        
        for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
            if(tasks[taskIndex].run == RUN) {
                if (tasks[taskIndex].counter >= tasks[taskIndex].frequency) {
                    trigger_event_data(tasks[taskIndex].event, tasks[taskIndex].argc, tasks[taskIndex].argv);
                    tasks[taskIndex].counter = 0;
                }
                tasks[taskIndex].counter++;
            }
        }
    }
}