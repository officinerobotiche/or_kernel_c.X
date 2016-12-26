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
/*	Include                                                                   */
/******************************************************************************/

#include "or_peripherals/SPI/24LC512.h"

/******************************************************************************/
/*  EEPROM Commands                                                           */
/******************************************************************************/

#define Lo( X )				( unsigned char )( X & 0x00FF )
#define Hi( X )				( unsigned char )( ( X >> 8 ) & 0x00FF )

#define EEPROM_PAGE_SIZE    (unsigned)64			//Size page
#define EEPROM_PAGE_MASK    (unsigned)0x003f

#define EEPROM_CMD_READ     (unsigned)0b00000011	//Read command
#define EEPROM_CMD_WRITE    (unsigned)0b00000010	//Write command
#define EEPROM_CMD_WRDI     (unsigned)0b00000100	//Disable write command
#define EEPROM_CMD_WREN     (unsigned)0b00000110	//Enable write command
#define EEPROM_CMD_RDSR     (unsigned)0b00000101	//Read STATUS command
#define EEPROM_CMD_WRSR     (unsigned)0b00000001	//Write STATUS command

/******************************************************************************/
/* Variable Declaration                                                       */
/******************************************************************************/

typedef struct _EEPROM_data {
    int command;
    uint16_t address;
} EEPROM_data_t;
#define LNG_EEPROM_DATA sizeof(EEPROM_data_t)

typedef union _EEPROMstatus {
    STATREG_t status;
    char bits;
} EEPROMstatus_t;

int BufferOut[LNG_EEPROM_DATA];

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

void EE24LC512_init(EE24LC512Conf_t* EEPROMConf) {
    // Initialization SPI chip select (CS) pin
    SPI_CS_Init(&EEPROMConf->CS);
}
/**
 * @brief Enable write operation inside the SPI EEPROM
 * @param EEPROMConf the Chip select of SPI EEPROM
 */
inline void EE24LC512_write_enable(EE24LC512Conf_t* EEPROMConf) {
    // Write the enable SPI command to the EEPROM
    SPI_read_write_single(EEPROMConf->SPI, &EEPROMConf->CS, EEPROM_CMD_WREN);
}
/**
 * @brief Disable write operation inside the SPI EEPROM
 * @param EEPROMConf the Chip select of SPI EEPROM
 */
inline void EE24LC512_write_disable(EE24LC512Conf_t* EEPROMConf) {
    // Write the disable SPI command to the EEPROM
    SPI_read_write_single(EEPROMConf->SPI, &EEPROMConf->CS, EEPROM_CMD_WRDI);
}
/**
 * @brief Read the status of the EEPROM
 * @param EEPROMConf
 * @return 
 */
inline STATREG_t EE24LC512_read_status(EE24LC512Conf_t* EEPROMConf) {
    EEPROMstatus_t statreg;
    // Get the SPI configuration
    SPI_type type = EEPROMConf->SPI->TYPE;
    // Set in 8bit mode the SPI
    SPI_setType(EEPROMConf->SPI, SPI_8bit);
    // Set low the chip select (CS) of the SPI controller
    SPI_CS_Low(&EEPROMConf->CS);
    // Write the EEPROM command to know the EEPROM status
    SPI_read_write(EEPROMConf->SPI, EEPROM_CMD_RDSR);
    // Get the EEPROM status
    statreg.bits = SPI_read_write(EEPROMConf->SPI, 0);
    // Restore the SPI configuration
    SPI_setType(EEPROMConf->SPI, type);
    // Set High the chip select (CS) of the SPI controller
    SPI_CS_High(&EEPROMConf->CS);
    // Return the EEPROM status
    return statreg.status;
}

bool EE24LC512_write_byte(EE24LC512Conf_t* EEPROMConf, uint16_t Address, int data) {
    // Send a single data to EEPROM 
    return EE24LC512_write((void*) EEPROMConf, Address, &data, 1, NULL);
}

bool EE24LC512_write_data(EE24LC512Conf_t* EEPROMConf, uint16_t Address, int *buff, size_t size) {
    return EE24LC512_write((void*) EEPROMConf, Address, buff, size, NULL);
}

bool EE24LC512_write(void* EEPROM, uint16_t Address, int *buff, size_t size, EE24LC512_cb cb) {
    EE24LC512Conf_t* _eeprom = (EE24LC512Conf_t*) EEPROM;
    unsigned int i;
    // Enable EEPROM write
    EE24LC512_write_enable(_eeprom);
    // Get the SPI configuration
    SPI_type type = _eeprom->SPI->TYPE;
    // Set in 8bit mode the SPI
    SPI_setType(_eeprom->SPI, SPI_8bit);
    // Set low the chip select (CS) of the SPI controller
    SPI_CS_Low(&_eeprom->CS);
    // Write the EEPROM command to write
    SPI_read_write(_eeprom->SPI, EEPROM_CMD_WRITE);
    // Write the address
    SPI_read_write(_eeprom->SPI, Hi(Address));
    SPI_read_write(_eeprom->SPI, Lo(Address));
    // Send all data
    for(i = 0; i < size; ++i) {
        SPI_read_write(_eeprom->SPI, buff[i]);
    }
    // Restore the SPI configuration
    SPI_setType(_eeprom->SPI, type);
    // Set High the chip select (CS) of the SPI controller
    SPI_CS_High(&_eeprom->CS);
    // wait for completion of previous write operation
    while(EE24LC512_read_status(_eeprom).WIP);
    // Disable EEPROM write
    EE24LC512_write_disable(_eeprom);
    return true;
}

uint8_t EE24LC512_read_byte(EE24LC512Conf_t* EEPROMConf, uint16_t Address) {
    // Data to store
    uint8_t temp = 0;
    // Read a single data
    EE24LC512_read((void*)EEPROMConf, Address, (int*) &temp, 1, NULL);
    // Return the data read
    return temp;
}

bool EE24LC512_read(void* EEPROM, uint16_t Address, int *buff, size_t size, EE24LC512_cb cb) {
    EE24LC512Conf_t* _eeprom = (EE24LC512Conf_t*) EEPROM;
    unsigned int i;
    // Get the SPI configuration
    SPI_type type = _eeprom->SPI->TYPE;
    // Set in 8bit mode the SPI
    SPI_setType(_eeprom->SPI, SPI_8bit);
    // Set low the chip select (CS) of the SPI controller
    SPI_CS_Low(&_eeprom->CS);
    // Write the EEPROM command to read
    SPI_read_write(_eeprom->SPI, EEPROM_CMD_READ);
    // Write the address
    SPI_read_write(_eeprom->SPI, Hi(Address));
    SPI_read_write(_eeprom->SPI, Lo(Address));
    // Write an empty data to maintain the clock enabled 
    for(i = 0; i < size; ++i) {
        buff[i] = SPI_read_write(_eeprom->SPI, 0);
    }
    // Set High the chip select (CS) of the SPI controller
    SPI_CS_High(&_eeprom->CS);
    // Restore the SPI configuration
    SPI_setType(_eeprom->SPI, type);
    return true;
}

bool EE24LC512_read_data(EE24LC512Conf_t* EEPROM, uint16_t Address, int *buff, size_t size) {
    return EE24LC512_read((void*) EEPROM, Address, buff, size, NULL);
}