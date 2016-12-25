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

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>

#include "or_system/spinlock.h"

/******************************************************************************/
/* System Level Functions                                                     */
/******************************************************************************/

inline void lock(lock_t *lock, bool state) {
    lock->lock = state;
}

inline void spin_lock(lock_t *lock, unsigned int level) {
    while (lock->lock);
    SET_AND_SAVE_CPU_IPL(lock->save_to, level);
}

inline void spin_unlock(lock_t *lock) {
    RESTORE_CPU_IPL(lock->save_to);
}
