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

#ifndef _EEPROM_H
#define _EEPROM_H

/******************************************************************************/
/*	Include																	  */
/******************************************************************************/

#include "or_peripherals/SPI/SPI.h"

/******************************************************************************/
/*	Define																	  */
/******************************************************************************/

typedef void (*EE24LC512_cb)(bool status);

typedef struct _STATREG {
    char WIP : 1;
    char WEL : 1;
    char BP0 : 1;
    char BP1 : 1;
    char RESERVED : 3;
    char WPEN : 1;
} STATREG_t;

typedef struct _EEPROMConf {
    SPI_CS_pin_t CS;
    SPI_conf_t *SPI;
} EE24LC512Conf_t;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * @brief Initialization EEPROM
 * @param EEPROMConf
 */
void EE24LC512_init(EE24LC512Conf_t* EEPROMConf);

/**
 * 
 * @param EEPROMConf
 * @param address
 * @param data
 */
bool EEPROM_write_single(EE24LC512Conf_t* EEPROMConf, uint16_t Address, int data);
/**
 * 
 * @param EEPROMConf
 * @param address
 * @param buff
 * @param size
 */
bool EE24LC512_write(void* EEPROM, uint16_t Address, int *buff, size_t size, EE24LC512_cb cb);
/**
 * 
 * @param EEPROM
 * @param Address
 * @param buff
 * @param size
 * @return 
 */
bool EE24LC512_write_data(EE24LC512Conf_t* EEPROM, uint16_t Address, int *buff, size_t size);
/**
 * 
 * @param EEPROMConf
 * @param Address
 * @return 
 */
uint8_t EE24LC512_read_byte(EE24LC512Conf_t* EEPROMConf, uint16_t Address);
/**
 * 
 * @param EEPROMConf
 * @param Address
 * @param buff
 * @param size
 */
bool EE24LC512_read(void* EEPROMConf, uint16_t Address, int *buff, size_t size, EE24LC512_cb cb);
/**
 * 
 * @param EEPROM
 * @param Address
 * @param buff
 * @param size
 * @return 
 */
bool EE24LC512_read_data(EE24LC512Conf_t* EEPROM, uint16_t Address, int *buff, size_t size);

#endif
