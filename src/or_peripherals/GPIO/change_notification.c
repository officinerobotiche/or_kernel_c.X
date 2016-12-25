/*
 * Copyright (C) 2014-2017 Officine Robotiche
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

#include "or_peripherals/GPIO/change_notification.h"
#include "or_system/events.h"

/******************************************************************************/
/* Definition                                                                 */
/******************************************************************************/

typedef struct _tagEventCN {
    gpio_cn_t *cn_pin;
    bool enable;
    hEvent_t event;
    gpio_cn_type type;
} CN_EVENT;

/******************************************************************************/
/* Variable Declaration                                                       */
/******************************************************************************/

/// Declare an array with all interrupts
CN_EVENT CN_event[MAX_CN_EVENT];

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

void gpio_cn_init() {
    hCNEvent_t cn_eventIndex;
    for(cn_eventIndex = 0; cn_eventIndex < MAX_CN_EVENT; ++cn_eventIndex) {
        CN_event[cn_eventIndex].cn_pin = NULL;
        CN_event[cn_eventIndex].type = CN_ALWAYS;
        CN_event[cn_eventIndex].enable = false;
        CN_event[cn_eventIndex].event = INVALID_EVENT_HANDLE;
    }
}

hCNEvent_t gpio_cn_register(gpio_cn_t *cn, gpio_cn_type type, hEvent_t event) {
    hCNEvent_t cn_eventIndex;
    for(cn_eventIndex = 0; cn_eventIndex < MAX_CN_EVENT; ++cn_eventIndex) {
        if(CN_event[cn_eventIndex].cn_pin == NULL) {
            CN_event[cn_eventIndex].cn_pin = cn;
            CN_event[cn_eventIndex].type = type;
            CN_event[cn_eventIndex].event = event;
            return cn_eventIndex;
        }
    }
    return INVALID_CN_EVENT_HANDLE;
}

void gpio_cn_pud(hCNEvent_t cn_eventIndex, bool enable) {
    if(cn_eventIndex != INVALID_CN_EVENT_HANDLE) {
        if(enable) {
            REGISTER_MASK_SET_HIGH(CN_event[cn_eventIndex].cn_pin->REG_PUD, CN_event[cn_eventIndex].cn_pin->CS_mask);
        } else {
            REGISTER_MASK_SET_LOW(CN_event[cn_eventIndex].cn_pin->REG_PUD, CN_event[cn_eventIndex].cn_pin->CS_mask);
        }
    }
}

void gpio_cn_enable(hCNEvent_t cn_eventIndex, bool enable) {
    if(cn_eventIndex != INVALID_CN_EVENT_HANDLE) {
        CN_event[cn_eventIndex].enable = enable;
        if(enable) {
            REGISTER_MASK_SET_HIGH(CN_event[cn_eventIndex].cn_pin->REG_CN, CN_event[cn_eventIndex].cn_pin->CS_mask);
        } else {
            REGISTER_MASK_SET_LOW(CN_event[cn_eventIndex].cn_pin->REG_CN, CN_event[cn_eventIndex].cn_pin->CS_mask);
        }
    }
}

inline void change_notification_controller() {
    hCNEvent_t cn_eventIndex;
    CN_EVENT *cnEvent;
    for (cn_eventIndex = 0; cn_eventIndex < MAX_CN_EVENT; ++cn_eventIndex) {
        cnEvent = &CN_event[cn_eventIndex];
        if (cnEvent->enable) {
            if (cnEvent->type == CN_ALWAYS) {
                // Run the event callback when the state is 
                trigger_event(cnEvent->event);
            } else if (gpio_get_pin(&cnEvent->cn_pin->gpio) == cnEvent->type) {
                // Run the event callback when the state is 
                trigger_event(cnEvent->event);
            }
        }
    }
}

