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

#include "system/task_manager.h"

/// Max number of task
#define MAX_TASKS 16
/// Max number of arguments
#define MAX_ARGV 2
/**
 * Definition of task:
 * Running or not
 * associated event
 * internal counter of event
 * frequency of esecution in [uS]
 * number of arguments
 * arguments
 */
typedef struct _tagTASK {
    task_status_t run;
    hEvent_t event;
    uint16_t counter;
    uint16_t counter_freq;
    frequency_t frequency;
    int argc;
    int argv[MAX_ARGV];
} TASK;

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/
/// Array of tasks
TASK tasks[MAX_TASKS];
/// Number of loaded tasks
unsigned short task_count = 0;
/// frequency TIMER
frequency_t FREQ_TIMER;
/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void task_init(frequency_t timer_frequency) {
    hTask_t taskIndex;
    FREQ_TIMER = timer_frequency;
    for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
        tasks[taskIndex].run = STOP;
        tasks[taskIndex].counter = 0;
        tasks[taskIndex].counter_freq = 0;
        tasks[taskIndex].frequency = 0;
        tasks[taskIndex].event = INVALID_EVENT_HANDLE;
        tasks[taskIndex].argc = 0;
    }
}

hTask_t task_load(hEvent_t hEvent, frequency_t frequency) {
    return task_load_data(hEvent, frequency, 0, NULL);
}

hTask_t task_load_data(hEvent_t hEvent, frequency_t frequency, int argc, ...) {
    hTask_t taskIndex;
    va_list argp;
    int argc_c = 0;
    if(frequency <= FREQ_TIMER && frequency > 0) {
        for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
            if (tasks[taskIndex].event == INVALID_EVENT_HANDLE) {
                tasks[taskIndex].run = STOP;
                tasks[taskIndex].event = hEvent;
                tasks[taskIndex].counter_freq = FREQ_TIMER / frequency;
                tasks[taskIndex].frequency = frequency;
                tasks[taskIndex].argc = argc;
                va_start(argp, argc);
                for(argc_c = 0; argc_c < argc; ++argc_c) {
                    tasks[taskIndex].argv[argc_c] = va_arg(argp, int);
                }
                task_count++;
                return taskIndex;
            }
        }
    }
    return INVALID_TASK_HANDLE;
}

bool task_set(hTask_t hTask, task_status_t run) {
    if(hTask != INVALID_TASK_HANDLE) {
        tasks[hTask].run = run;
        return true;
    }
    return false;
}

bool task_set_frequency(hTask_t hTask, frequency_t frequency) {
    if(hTask != INVALID_TASK_HANDLE) {
        if(frequency < FREQ_TIMER && frequency > 0) {
            tasks[hTask].counter_freq = FREQ_TIMER / frequency;
            tasks[hTask].frequency = frequency;
            return true;
        } else {
            return task_unload(hTask);
        }
    }
    return false;
}

bool task_unload(hTask_t hTask) {
    if(hTask != INVALID_TASK_HANDLE) {
        tasks[hTask].event = INVALID_EVENT_HANDLE;
        tasks[hTask].run = STOP;
        task_count--;
        return true;
    }
    return false;
}

hModule_t task_get_name(hTask_t taskIndex) {
    return get_event_name(tasks[taskIndex].event);
}

unsigned short get_task_number(void) {
    return task_count;
}

inline void task_manager(void) {
    if(task_count > 0) {
        hTask_t taskIndex;
        
        for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
            if(tasks[taskIndex].run == RUN) {
                if (tasks[taskIndex].counter >= tasks[taskIndex].counter_freq) {
                    trigger_event_data(tasks[taskIndex].event, tasks[taskIndex].argc, tasks[taskIndex].argv);
                    tasks[taskIndex].counter = 0;
                }
                tasks[taskIndex].counter++;
            }
        }
    }
}