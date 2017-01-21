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

#ifndef CHANGE_NOTIFICATION_H
#define	CHANGE_NOTIFICATION_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "or_peripherals/GPIO/gpio.h"
    
#ifndef MAX_CN_EVENT
#define MAX_CN_EVENT 10
#endif

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
    // Type of change notification event
    typedef enum {
        CN_RISING_NEG,
        CN_RISING_PLUS,
        CN_ALWAYS
    } gpio_cn_type;
    // Definition of change notification
    typedef struct {
        gpio_t gpio;
        REGISTER REG_CN;
        REGISTER REG_PUD;
        unsigned int CS_mask;
    } gpio_cn_t;
    // Initialization change notification pin
    #define GPIO_CN(x, n, reg_cn, reg_pud, m) { GPIO_INIT_IN(x, n), &(reg_cn), &(reg_pud), BIT_MASK((m)) }
    /// Invalid handle for change notification (CN) event
    #define INVALID_CN_EVENT_HANDLE 0xFFFF
    /// CN event register number
    typedef uint16_t hCNEvent_t;
    /// Callback when the change notification function start
    typedef void (*cn_callback_t)(void);
    
/******************************************************************************/
/*	FUNCTIONS							 									  */
/******************************************************************************/

    /**
     * @brief Initialization change notification (CN) controller
     */
    void gpio_cn_init();
    /**
     * @brief Register a change notification GPIO and add a reference callback
     * @param pin The change notification pin
     * @param cn The type of the change notification event. Read the 
     * @param type
     * @param callback
     * @param obj Pointer a data to recall
     * @return the number of the CN registered
     */
    hCNEvent_t gpio_cn_register(gpio_cn_t *cn, gpio_cn_type type, uint16_t event, void *obj);
    /**
     * 
     * @param cn_eventIndex
     * @param enable
     */
    void gpio_cn_pud(hCNEvent_t cn_eventIndex, bool enable);
    /**
     * 
     * @param cn_eventIndex
     * @param state
     */
    void gpio_cn_enable(hCNEvent_t cn_eventIndex, bool state);
    /**
     * Controller on every change notification
     */
    inline void change_notification_controller();


#ifdef	__cplusplus
}
#endif

#endif	/* CHANGE_NOTIFICATION_H */

