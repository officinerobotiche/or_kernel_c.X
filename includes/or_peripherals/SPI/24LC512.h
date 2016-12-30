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
#include "or_system/task_manager.h"

#ifndef EE24LC512_DEFAULT_FREQ_CHECK
#define EE24LC512_DEFAULT_FREQ_CHECK 500
#endif

/******************************************************************************/
/*	Define																	  */
/******************************************************************************/

typedef void (*EE24LC512_cb)(void *obj, bool status);

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
    hTask_t MeterCheck_task;
    EE24LC512_cb cb;
    void *obj;
} EE24LC512Conf_t;

#define EE24LC512CONF_INIT(CS, SPI) {CS, &(SPI), INVALID_TASK_HANDLE, NULL, NULL}

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * @brief Initialization EEPROM
 * @param EEPROMConf The configuration of EEPROM 25LC512
 */
void EE24LC512_init(EE24LC512Conf_t* EEPROMConf);
/**
 * @brief Write a single byte on EEPROM. This function recall inside EE24LC512_write.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to write
 * @param data The data
 */
bool EEPROM_write_single(EE24LC512Conf_t* EEPROMConf, uint16_t Address, int8_t data);
/**
 * @brief Write a buffer on EEPROM. This function recall inside EE24LC512_write.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to write
 * @param buff The buffer to write inside the EEPROM
 * @param size The size of the buffer
 * @return If the write success return true
 */
bool EE24LC512_write_data(EE24LC512Conf_t* EEPROM, uint16_t Address, int8_t *buff, size_t size);
/**
 * @brief Write a buffer on EEPROM. This function is compliant with data controller 
 * functions. You can use a callback when the data is completely written.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to write
 * @param buff The buffer to write inside the EEPROM
 * @param size The size of the buffer
 * @param cb If set the callback, this function will be no blocking
 * @param obj the pointer data to return the value
 * @return If no blocking return always true
 */
bool EE24LC512_write(void* EEPROM, uint16_t Address, int *buff, size_t size, EE24LC512_cb cb, void *obj);
/**
 * @brief Read a byte from EEPROM. This function recall inside EE24LC512_read.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to read
 * @return The data read
 */
uint8_t EE24LC512_read_byte(EE24LC512Conf_t* EEPROMConf, uint16_t Address);
/**
 * @brief Read a buffer from EEPROM. This function recall inside EE24LC512_read.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to read
 * @param buff The buffer to store the data read in EEPROM
 * @param size The size of the buffer
 */
bool EE24LC512_read_data(EE24LC512Conf_t* EEPROM, uint16_t Address, int8_t *buff, size_t size);
/**
 * @brief Read a buffer from EEPROM. This function is compliant with data controller 
 * functions. You can use a callback when the data is completely read.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to read
 * @param buff The buffer to store the data read in EEPROM
 * @param size The size of the buffer
 * @param cb If set the callback, this function will be no blocking
 * @param obj the pointer data to return the value
 * @return If no blocking return always true
 */
bool EE24LC512_read(void* EEPROMConf, uint16_t Address, int *buff, size_t size, EE24LC512_cb cb, void *obj);
/**
 * @brief Erase a page from EEPROM. This function is compliant with data controller 
 * functions. You can use a callback when the data is completely erase.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to erase
 * @param cb If set the callback, this function will be no blocking
 * @param obj the pointer data to return the value
 * @return If no blocking return always true
 */
bool EE24LC512_erase_page(void* EEPROM, uint16_t Address, EE24LC512_cb cb, void *obj);
/**
 * @brief Erase a sector from EEPROM. This function is compliant with data controller 
 * functions. You can use a callback when the data is completely erase.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param address The address to erase
 * @param cb If set the callback, this function will be no blocking
 * @param obj the pointer data to return the value
 * @return If no blocking return always true
 */
bool EE24LC512_erase_sector(void* EEPROM, uint16_t Address, EE24LC512_cb cb, void *obj);
/**
 * @brief Erase the EEPROM. This function is compliant with data controller 
 * functions. You can use a callback when the data is completely erase.
 * @param EEPROMConf The configuration of EEPROM 25LC512
 * @param cb If set the callback, this function will be no blocking
 * @param obj the pointer data to return the value
 * @return If no blocking return always true
 */
bool EE24LC512_erase_chip(void* EEPROM, EE24LC512_cb cb, void *obj);

#endif
