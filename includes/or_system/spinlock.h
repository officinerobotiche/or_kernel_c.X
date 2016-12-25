/*
 * Copyright (C) 2016 Officine Robotiche
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

#ifndef SPINLOCK_H
#define	SPINLOCK_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
    
/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
    typedef struct _lock {
        int save_to;
        bool lock;
    } lock_t;
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * Initialization lock variable
 * @param lock the lock structure
 * @param state The state of the lock variable True or False
 */
inline void lock(lock_t *lock, bool state);
/**
 * Lock the function after this function and lock all interrupt with a level minor
 * of "level" variable
 * @param lock the lock structure
 * @param level the level interrupt to lock
 */
inline void spin_lock(lock_t *lock, unsigned int level);
/**
 * After this function all interrupt are unlocked
 * @param lock the lock structure
 */
inline void spin_unlock(lock_t *lock);

#ifdef	__cplusplu
}
#endif

#endif	/* SPINLOCK_H */


