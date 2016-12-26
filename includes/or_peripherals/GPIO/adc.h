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

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "or_peripherals/GPIO/gpio.h"
    
#ifndef MAX_ADC_EVENT
#define MAX_ADC_EVENT 10
#endif
    
/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
    // Definition of ADC pin
    typedef struct {
        gpio_t gpio;
        REGISTER ANALOG;
        unsigned int CS_mask;
        int value;
        unsigned int start;
        unsigned int length;
    } gpio_adc_t;
    // Initialization change notification pin
    #define GPIO_ADC(x, n, reg_adc, m, s, g) { GPIO_INIT_IN(x, n), &(reg_adc), BIT_MASK((m)), 0, s, g }
    /// Invalid handle for change notification (CN) event
    #define INVALID_ADC_EVENT_HANDLE 0xFFFF
    /// CN event register number
    typedef uint16_t hADCEvent_t;
    /// Callback when the change notification function start
    typedef void (*adc_callback_t)(void * obj);
    
/******************************************************************************/
/*	FUNCTIONS							 									  */
/******************************************************************************/

    void gpio_adc_init(REGISTER analog);
    
    hADCEvent_t gpio_adc_register(gpio_adc_t *adc, size_t size, adc_callback_t cb, void* obj);
    
    void gpio_adc_enable(hADCEvent_t adc_eventIndex, bool enable);
    
    inline void ADC_controller(unsigned int *buffer);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

