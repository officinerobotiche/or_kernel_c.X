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
    
    void task_init(void);
    
    bool task_load(hEvent_t hEvent, frequency_t frequency);
    
    bool task_unload(hEvent_t hEvent);

    inline void task_manager(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TASK_MANAGER_H */

