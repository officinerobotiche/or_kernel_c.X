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
/* Files to Include                                                           */
/******************************************************************************/

#include "or_peripherals/SPI/SPI.h"

#define SPI_STAT_ENABLE BIT_MASK(15)
#define SPI_STAT_MASK_ROV BIT_MASK(6)
#define SPI_STAT_MASK_TBF BIT_MASK(1)
#define SPI_STAT_MASK_RBF BIT_MASK(0)

#define SPI_CON1_MASK_MODE16 BIT_MASK(10)
#define SPI_CON1_MASK_MASTER BIT_MASK(5)

#define SPI_CON1_MASK_CKE BIT_MASK(8)
#define SPI_CON1_MASK_CKP BIT_MASK(6)

#define SPI_CON1_MASK_PRESCALER  0b11
#define SPI_CON1_MASK_POSTSCALER 0b11100

#define SPI_DEFAULT_TIMEOUT     1000                //in [Hz]

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

void SPI_init(SPI_t *SPI, bool isMaster, bool enable) {
    // Set the SPI in master mode
    if(isMaster) {
        REGISTER_MASK_SET_HIGH(SPI->SPICON1, SPI_CON1_MASK_MASTER);
    } else {
        REGISTER_MASK_SET_LOW(SPI->SPICON1, SPI_CON1_MASK_MASTER);
    }
    // Initialization lock variable
    SPI->lock = false;
    // Set OK SPI
    SPI->status = SPI_OK;
    // Enable SPI
    SPI_enable(SPI, enable);
}

void SPI_config(SPI_t *SPI, SPI_conf_t *conf) {
    // Get SPI status
    bool enable = SPI_isEnable(SPI);
    // Disable the SPI controller
    SPI_enable(SPI, false);
    // Set the primary pre scaler
    REGISTER_MASK_SET_HIGH(SPI->SPICON1, conf->primary_prescaler);
    // Set the secondary pre scaler
    REGISTER_MASK_SET_HIGH(SPI->SPICON1, conf->secondary_prescaler << 2);
    // Set the type of communication
    SPI_setType(SPI, conf->TYPE);
    // Setup CKE and CKP
    if(conf->cke) {
        REGISTER_MASK_SET_HIGH(SPI->SPICON1, SPI_CON1_MASK_CKE);
    } else {
        REGISTER_MASK_SET_LOW(SPI->SPICON1, SPI_CON1_MASK_CKE);
    }
    if(conf->ckp) {
        REGISTER_MASK_SET_HIGH(SPI->SPICON1, SPI_CON1_MASK_CKP);
    } else {
        REGISTER_MASK_SET_LOW(SPI->SPICON1, SPI_CON1_MASK_CKP);
    }
    //set again SPI status
    SPI_enable(SPI, enable);
}

inline void SPI_setType(SPI_t *SPI, SPI_type type) {
    if(type == SPI_16bit) {
        REGISTER_MASK_SET_HIGH(SPI->SPICON1, SPI_CON1_MASK_MODE16);
    } else {
        REGISTER_MASK_SET_LOW(SPI->SPICON1, SPI_CON1_MASK_MODE16);
    }
}

inline void SPI_enable(SPI_t *SPI, bool enable) {
    if(enable) {
        REGISTER_MASK_SET_HIGH(SPI->SPISTAT, SPI_STAT_ENABLE);
    } else {
        REGISTER_MASK_SET_LOW(SPI->SPISTAT, SPI_STAT_ENABLE);
    }
}

inline bool SPI_isEnable(SPI_t *SPI) {
    return REGISTER_MASK_READ(SPI->SPISTAT, SPI_STAT_ENABLE);
}

void SPI_CS_Init(SPI_CS_pin_t *CS) {
    // Configuration pin chip select
    gpio_init_pin(&CS->port);
    // Set in High mode
    SPI_CS_High(CS);
}

inline void __attribute__((always_inline)) SPI_CS_High(SPI_CS_pin_t *CS) {
    if (!CS->isInverted) {
        gpio_set_pin(&CS->port, GPIO_HIGH);
    } else {
        gpio_set_pin(&CS->port, GPIO_LOW);
    }
}

inline void __attribute__((always_inline)) SPI_CS_Low(SPI_CS_pin_t *CS) {
    if (!CS->isInverted) {
        gpio_set_pin(&CS->port, GPIO_LOW);
    } else {
        gpio_set_pin(&CS->port, GPIO_HIGH);
    }
}

inline void __attribute__((always_inline)) SPI_lock(SPI_t *SPI, bool lock) {
    if(lock) {
        if(SPI->lock) {
            while(SPI->lock);
        }
        SPI->lock = lock;
    } else {
        SPI->lock = lock;
    }
}

void SPI_flush(SPI_t *SPI) {
    SPI->status = SPI_OK;
}

inline void __attribute__((always_inline)) SPI_error(SPI_t *SPI) {
    // Clear SPI ROV error register
    REGISTER_MASK_SET_LOW(SPI->SPISTAT, SPI_STAT_MASK_ROV);
    // Set in ERROR the SPI
    SPI->status = SPI_ERROR;
}

inline int __attribute__((always_inline)) SPI_read_write(SPI_t *SPI, int data) {
    // write to buffer for TX
    *SPI->SPIBUFF = data;
    // wait for transfer to complete
    unsigned int count = 0;
    while(!REGISTER_MASK_READ(SPI->SPISTAT, SPI_STAT_MASK_RBF)) {
        if(++count > 200) {
            SPI->status = SPI_TIMEOUT;
            return 0;
        }
    }
    // read the received value
    return *SPI->SPIBUFF;
}

inline void __attribute__((always_inline)) SPI_write(SPI_t *SPI, int data) {
    // write to buffer for TX
    *SPI->SPIBUFF = data;
    // wait for transfer to complete
    while(!REGISTER_MASK_READ(SPI->SPISTAT, SPI_STAT_MASK_TBF));
}

inline void __attribute__((always_inline)) SPI_read_write_buff_new(SPI_t *SPI, SPI_CS_pin_t *CS, int *buff_in, int *buff_out, size_t size) {
    unsigned int i;
    // Put low chip select line
    SPI_CS_Low(CS);
    // If the pointer is NULL only write in the SPI
    if(buff_out != NULL) {
        // loop to dump all buffer from SPI
        for(i = 0; i < size; ++i) {
            buff_out[i] = SPI_read_write(SPI, buff_in[i]);
        }
    } else {
        // loop to dump all buffer from SPI
        for(i = 0; i < size; ++i) {
            SPI_read_write(SPI, buff_in[i]);
        }
    }
    // Put High chip select line
    SPI_CS_High(CS);
}

inline void __attribute__((always_inline)) SPI_read_write_buff(SPI_t *SPI, SPI_CS_pin_t *CS, int8_t *buff_in, int8_t *buff_out, size_t size, size_t size_data) {
    unsigned int i;
    // Put low chip select line
    SPI_CS_Low(CS);
    // loop to dump all buffer from SPI
    for(i = 0; i < size; ++i) {
        // TODO to improve
        int step = i / size_data;
        int pos = (size_data - i - 1) % size_data;
        int val = step*size_data + pos;
        buff_out[val] = SPI_read_write(SPI, buff_in[val]);
    }
    // Put High chip select line
    SPI_CS_High(CS);
}

int SPI_read_write_single(SPI_t *SPI, SPI_CS_pin_t *CS, int data) {
    // Put low chip select line
    SPI_CS_Low(CS);
    // write to buffer for TX
    int data_out = SPI_read_write(SPI, data);
    // Put High chip select line
    SPI_CS_High(CS);
    // read the received value
    return data_out;
}

void SPI_write_single(SPI_t *SPI, SPI_CS_pin_t *CS, int data) {
    // Put low chip select line
    SPI_CS_Low(CS);
    // write to buffer for TX
    SPI_write(SPI, data);
    // Put High chip select line
    SPI_CS_High(CS);
}