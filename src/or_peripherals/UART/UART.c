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

#include "or_peripherals/UART/UART.h"

#define UART_STAT_MASK_TRMT  BIT_MASK(8)
#define UART_STAT_MASK_FERR  BIT_MASK(2)
#define UART_STAT_MASK_OERR  BIT_MASK(1)
#define UART_STAT_MASK_URXDA BIT_MASK(0)

#define UART_MODE_MASK_PDSEL_3 0b0000000000000110

/******************************************************************************/
/* Variable Declaration                                                       */
/******************************************************************************/

inline void UART_read_callback (int argc, int* argv);

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

void UART_register_write(UART_WRITE_t *write, REGISTER UARTTX, UART_ext_write cb) {
    unsigned int i;
    // Initialization register and variables
    write->cb = cb;
    write->UARTTX = UARTTX;
    write->lock = false;
    // Initialize buffer UART queue
    for(i = 0; i < LNG_UART_TX_QUEUE; ++i) {
        write->queque[i].pending = false;
        write->queque[i].size = 0;
    }
    // Initialize UART queue counter
    write->queue_counter = 0;
}

void UART_register_read(UART_READ_t *read, REGISTER port, REGISTER reg, unsigned int x, UART_read_cb cb) {
    // Initialization event run callback
    read->read_event = register_event_p(&UART_read_callback, EVENT_PRIORITY_HIGH);
    read->UARTRX = port;
    // Initialize interrupt port
    read->UART_INT.REG = reg;
    read->UART_INT.CS_mask = BIT_MASK(x);
    read->UART_read_cb = cb;
    // reset buffer out RX pointer
    read->buff_rx_out = 0;
    // reset buffer in RX pointer
    read->buff_rx_in = 0;
}

bool UART_setBaudrate(UART_t* UART, unsigned long baudrate) {
    *UART->UBRG = ((UART->fcy/baudrate)/16)-1;  // Baud rate initialization
    return true;
}

inline bool UART_isLocked(UART_t* UART) {
    return UART->write->lock;
}

inline UART_state_t UART_store(UART_t* UART, unsigned char* buff, size_t size) {
    unsigned int i;
    if (size < LNG_UART_TX_QUEUE_BUFF) {
        if (UART->write->queue_counter < LNG_UART_TX_QUEUE) {
            for (i = 0; i < LNG_UART_TX_QUEUE; ++i) {
                if (UART->write->queque[i].pending == false) {
                    UART->write->queque[i].pending = true;
                    memmove(&UART->write->queque[i].buff, buff, size);
                    UART->write->queque[i].size = size;
                    UART->write->queue_counter++;
                    return UART_STATE_PENDING;
                }
            }
        }
    }
    return UART_STATE_FALSE;
}

inline void UART_write_in_port(UART_t* UART, unsigned char* buff, size_t size) {
    unsigned int i;
    //Lock the UART communication
    UART->write->lock = true;
    for (i = 0; i < size; i++) {
        // Write a data in buffer
        *UART->write->UARTTX = buff[i];
        // Check if transmit Shift register is empty
        while(REGISTER_MASK_READ(UART->UARTSTA, UART_STAT_MASK_TRMT) == 0);
    }
    // Unlock the UART write
    UART->write->lock = false;
}

inline void UART_serve_queue(UART_t* UART) {
    unsigned int i;
    if (UART->write->queue_counter < LNG_UART_TX_QUEUE) {
        for(i = 0; i < LNG_UART_TX_QUEUE; ++i) {
            // If pending run the callback
            if(UART->write->queque[i].pending == true) {
                // If the external write is initialized run the write controller outside
                if(UART->write->cb != NULL) {
                    UART->write->cb(UART, UART->write->queque[i].buff, UART->write->queque[i].size);
                } else {
                    UART_write_in_port(UART, UART->write->queque[i].buff, UART->write->queque[i].size);
                }
                UART->write->queue_counter--;
                UART->write->queque[i].pending = false;
            }
        }
    }
}

inline UART_state_t UART_write(UART_t* UART, unsigned char* buff, size_t size) {
    // Check if UART communication is blocked
    if(UART->write->lock) {
        // If locked store the data to send in buffer
        return UART_store(UART, buff, size);
    } else {
        // otherwise send all data in UART port
        if(UART->write->cb != NULL) {
            // If the external write is initialized run the write controller outside
            UART->write->cb(UART, buff, size);
        } else {
            UART_write_in_port(UART, buff, size);
        }
        // Send all stored data
        while(UART->write->queue_counter > 0) {
            UART_serve_queue(UART);
        }
        return UART_STATE_TRUE;
    }
}

inline void UART_read_callback (int argc, int* argv) {
    UART_t* UART = (UART_t*) &argv[0];
    // Lock reader
    UART->read->lock = true;
    // Run callback for all char inside the buffer
    while(UART->read->buff_rx_out != UART->read->buff_rx_in) {
        unsigned char rxdata = UART->read->buff[UART->read->buff_rx_out];
        // Launch the read callback
        UART->read->UART_read_cb(rxdata);
        // Update buffer RX out pointer
        UART->read->buff_rx_out = 
                ((UART->read->buff_rx_out + 1) % LNG_UART_RX_QUEUE);
    }
    // Unlock reader
    UART->read->lock = false;
}

inline void UART_read(UART_t* UART) {
    /* check for receive errors */
    if(REGISTER_MASK_READ(UART->UARTSTA, UART_STAT_MASK_FERR) == 1) {
        // TODO Add error checking
    }
    /* get the data */
    if (REGISTER_MASK_READ(UART->UARTSTA, UART_STAT_MASK_OERR) == 0 && REGISTER_MASK_READ(UART->UARTSTA, UART_STAT_MASK_URXDA) == 1) {
        // Read a char and decode a message
        unsigned int rxdata;
        // Correction register UART2 with PDSEL selection
        if (REGISTER_MASK_READ(UART->UARTMODE, UART_MODE_MASK_PDSEL_3)) {
            rxdata = *UART->read->UARTRX;
        } else {
            rxdata = *UART->read->UARTRX & 0xFF;
        }
        // Add in buffer the new char read
        UART->read->buff[UART->read->buff_rx_in] = rxdata;
        // Increase the counter
        unsigned int count = ((UART->read->buff_rx_in + 1) % LNG_UART_RX_QUEUE);
        if(count != UART->read->buff_rx_out) {
            UART->read->buff_rx_in = count;
        }
        if(UART->read->lock == false) {
            // Run the read event
            trigger_event_data(UART->read->read_event, 1, (int*) UART);
        }
    } else {
        /* must clear the overrun error to keep UART receiving */
        if (REGISTER_MASK_READ(UART->UARTSTA, UART_STAT_MASK_OERR) == 1) {
            REGISTER_MASK_SET_LOW(UART->UARTSTA, UART_STAT_MASK_OERR);
            // TODO Add error checking
        }
    }
    // Clear RX interrupt flag
    REGISTER_MASK_SET_LOW(UART->read->UART_INT.REG, UART->read->UART_INT.CS_mask);
}