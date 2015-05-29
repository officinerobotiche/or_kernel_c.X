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
    hEvent_t event;
    uint16_t counter;
    frequency_t frequency;
} TASK;

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

TASK tasks[MAX_TASKS];

/*****************************************************************************/
/* Communication Functions                                                   */
/*****************************************************************************/

void task_init(void) {
    hTask_t taskIndex;
    
    for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
        tasks[taskIndex].counter = 0;
        tasks[taskIndex].frequency = 0;
        tasks[taskIndex].event = NULL;
    }
}

bool task_load(hEvent_t hEvent, frequency_t frequency) {
    hTask_t taskIndex;
    
    for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
        if (tasks[taskIndex].event == NULL) {
            tasks[taskIndex].event = hEvent;
            tasks[taskIndex].event = frequency;
            return true;
        }
    }
    return false;
}

bool task_unload(hEvent_t hEvent) {
    hTask_t taskIndex;
    
    for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
        if (tasks[taskIndex].event == hEvent) {
            tasks[taskIndex].event = NULL;
            return true;
        }
    }
    return false;
}

inline void task_manager(void) {
    hTask_t taskIndex;
    
    for (taskIndex = 0; taskIndex < MAX_TASKS; ++taskIndex) {
        if (tasks[taskIndex].counter >= tasks[taskIndex].frequency) {
            trigger_event(tasks[taskIndex].event);
            tasks[taskIndex].counter = 0;
        }
        tasks[taskIndex].counter++;
    }
}