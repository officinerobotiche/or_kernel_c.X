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

#ifndef MODULES_H
#define	MODULES_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    
#include <stdint.h>        /* Includes uint16_t definition                    */
#include "data/data.h"
    
    /// Invalid handle for event
    #define INVALID_MODULE_HANDLE 0xFFFF

    /// Module register number
    typedef uint16_t hModule_t;
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

hModule_t register_module(string_data_t* name);



#ifdef	__cplusplus
}
#endif

#endif	/* MODULES_H */

