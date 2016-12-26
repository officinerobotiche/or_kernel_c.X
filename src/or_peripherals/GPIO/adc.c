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

#include "or_peripherals/GPIO/adc.h"

#include "or_system/events.h"

/******************************************************************************/
/* Definition                                                                 */
/******************************************************************************/

typedef struct _tagEventADC {
    gpio_adc_t *adc_pin;
    size_t size;
    adc_callback_t cb;
    void *obj;
} ADC_EVENT;

/******************************************************************************/
/* Variable Declaration                                                       */
/******************************************************************************/

/// Declare an array with all interrupts
ADC_EVENT ADC_event[MAX_ADC_EVENT];

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/


void gpio_adc_init(REGISTER analog) {
    hADCEvent_t adc_eventIndex;
    for(adc_eventIndex = 0; adc_eventIndex < MAX_ADC_EVENT; ++adc_eventIndex) {
        ADC_event[adc_eventIndex].adc_pin = NULL;
    }
    // Default set low digital all ADC pins
    REGISTER_MASK_SET_HIGH(analog, 0xFFFF);
}

hADCEvent_t gpio_adc_register(gpio_adc_t *adc, size_t size, adc_callback_t cb, void* obj) {
    hADCEvent_t adc_eventIndex;
    unsigned int i;
    for(adc_eventIndex = 0; adc_eventIndex < MAX_ADC_EVENT; ++adc_eventIndex) {
        if(ADC_event[adc_eventIndex].adc_pin == NULL) {
            ADC_event[adc_eventIndex].adc_pin = adc;
            ADC_event[adc_eventIndex].size = size;
            ADC_event[adc_eventIndex].cb = cb;
            ADC_event[adc_eventIndex].obj = obj;
            // Initialize all ADC GPIO and set output
            for(i = 0; i < ADC_event[adc_eventIndex].size; ++i) {
                gpio_init_pin(&ADC_event[adc_eventIndex].adc_pin[i].gpio);
                gpio_set_pin(&ADC_event[adc_eventIndex].adc_pin[i].gpio, GPIO_OUTPUT);
            }
            return adc_eventIndex;
        }
    }
    return INVALID_ADC_EVENT_HANDLE;
}

void gpio_adc_enable(hADCEvent_t adc_eventIndex, bool enable) {
    unsigned int i;
    if(adc_eventIndex != INVALID_ADC_EVENT_HANDLE) {
        for(i = 0; i < ADC_event[adc_eventIndex].size; ++i) {
            if(enable) {
                REGISTER_MASK_SET_LOW(ADC_event[adc_eventIndex].adc_pin[i].ANALOG, 
                        ADC_event[adc_eventIndex].adc_pin[i].CS_mask);
            } else {
                REGISTER_MASK_SET_HIGH(ADC_event[adc_eventIndex].adc_pin[i].ANALOG, 
                        ADC_event[adc_eventIndex].adc_pin[i].CS_mask);
            }
        }
    }
}

inline void ADC_controller(unsigned int *buffer) {
    hADCEvent_t adc_eventIndex;
    ADC_EVENT *adcEvent;
    unsigned int i, adc_pinIndex;
    unsigned long temp;
    for (adc_eventIndex = 0; adc_eventIndex < MAX_ADC_EVENT; ++adc_eventIndex) {
        adcEvent = &ADC_event[adc_eventIndex];
        for(adc_pinIndex = 0; adc_pinIndex < adcEvent->size; ++adc_pinIndex) {
            for(i = 0; i < adcEvent->adc_pin[adc_pinIndex].length; ++i) {
                temp += (buffer)[i+adcEvent->adc_pin[adc_pinIndex].start];
            }
            adcEvent->adc_pin[adc_pinIndex].value /= adcEvent->adc_pin[adc_pinIndex].length;
        }
        // Run the ADC event
        adcEvent->cb(adcEvent->obj);
    }
}