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

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/*	Include																	  */
/******************************************************************************/
    
    #include <stdbool.h>
    #include "or_peripherals/GPIO/gpio.h"
    #include "or_system/task_manager.h"

    typedef enum {
        SPI_8bit = 1,
        SPI_16bit = 2
    } SPI_type;
    
    typedef enum {
        SPI_OK,
        SPI_ERROR,
        SPI_TIMEOUT
    } SPI_status_t;
    
    typedef struct _SPI_conf {
        SPI_type TYPE;
        bool cke;
        bool ckp;
        int primary_prescaler;
        int secondary_prescaler;
        frequency_t clock;
    } SPI_conf_t;
    
#define SPI_CONF_INIT(type, cke, ckp, p, s) {type, cke, ckp, p, s, 0}
    
    typedef struct _SPI {
        REGISTER SPISTAT;
        REGISTER SPICON1;
        REGISTER SPIBUFF;
        volatile bool lock;
        SPI_status_t status;
    } SPI_t;
    
#define SPI_INIT(SPIstat, SPIcon1, SPIbuff) {&(SPIstat), &(SPIcon1), &(SPIbuff), false, INVALID_TASK_HANDLE}

    /**
     * Definition of the chip select pin
     */
    typedef struct _CS_conf {
        // definition of the port
        gpio_t port;
        // if the chip select is invert change control
        bool isInverted;
    } SPI_CS_pin_t;
    
#define SPI_CS_INIT(x, n, isInverted) {GPIO_INIT_OUT(x, n), isInverted}
#define SPI_CS_INIT_LONG(PORT, isInverted) {PORT, isInverted}
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/**
 * @brief Initialization SPI controller
 * @param SPI
 * @param isMaster
 * @param enable
 */
void SPI_init(SPI_t *SPI, bool isMaster, bool enable);
/**
 * 
 * @param SPI
 * @param conf
 */
void SPI_config(SPI_t *SPI, SPI_conf_t *conf);
/**
 * @brief set type of SPI
 * @param type
 */
void SPI_setType(SPI_t *SPI, SPI_type type);
/**
 * @brief Enable the SPI controller
 * @param enable set the state of the SPI controller
 */
void SPI_enable(SPI_t *SPI, bool enable);
/**
 * @brief Read the state of the SPI controller
 * @return the state of the SPI controller
 */
bool SPI_isEnable(SPI_t *SPI);
/**
 * @brief Set locking communication
 * @param SPI
 * @param lock
 */
inline void SPI_lock(SPI_t *SPI, bool lock);
/**
 * 
 * @param data
 * @return 
 */
inline int SPI_read_write(SPI_t *SPI, int data);
/**
 * 
 * @param CS
 * @param buff_in
 * @param buff_out
 * @param size
 * @param size_data
 */
void SPI_read_write_buff_new(SPI_t *SPI, SPI_CS_pin_t *CS, int *buff_in, int *buff_out, size_t size);
/**
 * 
 * @param CS
 * @param buff_in
 * @param buff_out
 * @param size
 */
void SPI_read_write_buff(SPI_t *SPI, SPI_CS_pin_t *CS, int8_t *buff_in, int8_t *buff_out, size_t size, size_t size_data);
/**
 * 
 * @param CS
 * @param data
 * @return 
 */
int SPI_read_write_single(SPI_t *SPI, SPI_CS_pin_t *CS, int data);
/**
 * @brief Initialization chip select
 * @param CS SPI chip select pin
 */
void SPI_CS_Init(SPI_CS_pin_t *CS);
/**
 * @brief Set High value the pin of the chip select
 * @param CS SPI chip select pin
 */
void SPI_CS_High(SPI_CS_pin_t *CS);
/**
 * @brief Set low the pin of the chip select
 * @param CS SPI chip select pin
 */
void SPI_CS_Low(SPI_CS_pin_t *CS);

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

