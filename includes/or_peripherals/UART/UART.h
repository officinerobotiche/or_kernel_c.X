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

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************/
/*	Include																	  */
/******************************************************************************/
    
    #include <stdbool.h>
    #include <string.h>
    #include "or_peripherals/GPIO/gpio.h"
    #include "or_system/events.h"

#ifndef LNG_UART_TX_QUEUE
#define LNG_UART_TX_QUEUE 4
#endif
    
#ifndef LNG_UART_RX_QUEUE
#define LNG_UART_RX_QUEUE 20
#endif

    typedef enum {
        UART_STATE_FALSE,
        UART_STATE_TRUE,
        UART_STATE_PENDING,
    } UART_state_t;
    
    typedef struct _UARTbuff {
        bool pending;
        unsigned char *buff;
        size_t size;
    } UARTbuff_t;
    
    typedef struct _UART_WRITE {
        REGISTER UARTTX;
        bool lock;
        unsigned int queue_counter;
        UARTbuff_t queque[LNG_UART_TX_QUEUE];
    } UART_WRITE_t;
    
    typedef void (*UART_read_cb)(unsigned char rxdata);
    
    typedef struct _UART_READ {
        REGISTER UARTRX;
        hardware_bit_t UART_INT;
        hEvent_t read_event;
        unsigned char buff[LNG_UART_RX_QUEUE];
        unsigned int buff_rx_in;
        unsigned int buff_rx_out;
        bool lock;
        UART_read_cb UART_read_cb;
    } UART_READ_t;
    
    typedef struct _UART {
        REGISTER UARTSTA;
        REGISTER UARTMODE;
        UART_WRITE_t *write;
        UART_READ_t *read;
    } UART_t;
    
#define UART_INIT(status, mode, write, read) {&(status), &(mode), write, read}
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
/**
 * @breif registration UART write controller
 * @param write The structure with write UART configuration
 * @param UARTTX The Transmitter register
 */
void UART_register_write(UART_WRITE_t *write, REGISTER UARTTX);
/**
 * 
 * @param read
 * @param port
 * @param reg
 * @param x
 * @param cb
 */
void UART_register_read(UART_READ_t *read, REGISTER port, REGISTER reg, unsigned int x, UART_read_cb cb);
/**
 * @brief The UART write controller. Required UART write configured.
 * @param UART The structure with UART configuration
 * @param buff The pointer of buffer to send
 * @param size the size of buffer
 * @return The state of UART controller
 */
inline UART_state_t UART_write(UART_t* UART, unsigned char* buff, size_t size);
/**
 * @brief The UART READ controller. Required UART read configured.
 * @param UART The structure with UART configuration
 */
inline void UART_read(UART_t* UART);
    
#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */
