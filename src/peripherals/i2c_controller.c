/*
 * Copyright (C) 2014-2015 Officine Robotiche
 * Authors: Guido Ottaviani, Raffaello Bonghi
 * email:  guido@guiott.com, raffaello.bonghi@officinerobotiche.it
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
 * 
 * Original code:
 * https://code.google.com/p/gentlenav/source/browse/trunk/libUDB/I2C1.c
 */

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <stdbool.h>       /* Includes true/false definition */

#include "peripherals/i2c_controller.h"
#include "system/modules.h"

/// Define mask type of bit
#define MASK_I2CCON_EN           BIT_MASK(15)
#define MASK_I2CCON_ACKDT        BIT_MASK(5)
#define MASK_I2CCON_ACKEN        BIT_MASK(4)
#define MASK_I2CCON_RCEN         BIT_MASK(3)
#define MASK_I2CCON_PEN          BIT_MASK(2)
#define MASK_I2CCON_SEN          BIT_MASK(0)

#define MASK_I2CSTAT_ACKSTAT     BIT_MASK(15)

#define I2C_COMMAND_WRITE 0
#define I2C_COMMAND_READ 1

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

void I2C_load(void);
void I2C_reset(void);
void I2C_serve_queue(void);
inline bool I2C_CheckAvailable(void);
void I2C_startWrite(void);
void I2C_writeCommand(void);
void I2C_writeCommandData(void);

/* READ FUNCTIONS */
void I2C_readStart(void);
void I2C_readCommand(void);
void I2C_recen(void);
void I2C_recstore(void);
void I2C_stopRead(void);
void I2C_rerecen(void);
void I2C_doneRead(void);

/* WRITE FUNCTIONS */
void I2C_writeData(void);   
void I2C_writeStop(void);
void I2C_doneWrite(void);

/* SERVICE FUNCTIONS */
void I2C_idle(void);
void I2C_Failed(void);
bool I2C_Normal(void);
void I2C_trigger_service(void);
    
#define I2C "I2C"
static string_data_t _MODULE_I2C = {I2C, sizeof (I2C)};
/// define depth queue
#define I2C_QUEUE_DEPTH     3
/// Define I2C queue
typedef struct tag_I2Cqueue {
    bool pending;                   ///< If the I2C message is in pending
    bool rW;                        ///< type of message
    unsigned char command;          ///< Command
    unsigned char* pcommandData;    ///< Command data
    unsigned char commandDataSize;  ///< Size of command data
    unsigned char* pData;           ///< Pointer to additional data
    unsigned int Size;              ///< Size of additional data
    I2C_callbackFunc pCallback;     ///< Callback
} I2Cqueue;
/// Buffer I2C queue
I2Cqueue i2c_queue[I2C_QUEUE_DEPTH];
/// Size I2C queue
int I2CMAXQ = 0;
/// I2C service event
static hEvent_t I2C_service_handle = INVALID_EVENT_HANDLE;
/// Pointer to function
void (* I2C_state) (void) = &I2C_idle;
// Port busy flag.  Set true until initialized
bool I2C_Busy = true;
/// index into the write buffer
unsigned int I2C_Index = 0; 
/// Callback to return
I2C_callbackFunc pI2C_callback = NULL;
/// Data size to send or receive a message
typedef struct _I2C_data_size {
    unsigned int tx;
    unsigned int rx;
} I2C_data_size_t;
I2C_data_size_t I2C_data_size = {0, 0};
/// Type of error
int I2C_ERROR = 0;

unsigned char I2C_CommandByte = 0;
unsigned int I2C_command_data_size = 0; ///< command data size

unsigned char* pI2CBuffer = NULL; ///< pointer to buffer
unsigned char* pI2CcommandBuffer = NULL; ///< pointer to receive  buffer

hardware_bit_t* I2C_INTERRUPT;
REGISTER I2C_CON;
REGISTER I2C_STAT;
REGISTER I2C_TRN;
REGISTER I2C_RCV;
I2C_callbackFunc res_Callback = NULL;

/******************************************************************************/
/* Parsing functions                                                          */
/******************************************************************************/
/**
 * Trigger the I2C controller event
 */
void I2C_trigger_service(void) {
    trigger_event(I2C_service_handle);
}
/**
 * Default operation when I2C event is launched
 * @param argc unused
 * @param argv unused
 */
void serviceI2C(int argc, int* argv) {
    if (REGISTER_MASK_READ(I2C_CON, MASK_I2CCON_EN) == 0) ///< I2C is off
    {
        I2C_state = &I2C_idle; ///< disable response to any interrupts
        I2C_load(); //< turn the I2C back on
        ///< Put something here to reset state machine.  Make sure attached services exit nicely.
    }
}

inline void I2C_manager (void) {
    (* I2C_state) (); // execute the service routine
    return;
}

void I2C_Init(hardware_bit_t* i2c_interrupt, REGISTER i2c_con, REGISTER i2c_stat, REGISTER i2c_trn, REGISTER i2c_rcv, I2C_callbackFunc resetCallback) {

    I2C_INTERRUPT = i2c_interrupt;
    I2C_CON = i2c_con;
    I2C_STAT = i2c_stat;
    I2C_TRN = i2c_trn;
    I2C_RCV = i2c_rcv;
    res_Callback = resetCallback;
    /// Register module
    hModule_t i2c_module = register_module(&_MODULE_I2C);
    /// Register event
    I2C_service_handle = register_event_p(i2c_module, &serviceI2C, EVENT_PRIORITY_LOW);
    
    I2C_load();
    return;
}

/**
 * Initialize the I2C queue buffer and reset state of I2C controller
 */
void I2C_load(void) {
    int queueIndex;

    for (queueIndex = 0; queueIndex < I2C_QUEUE_DEPTH; queueIndex++) {
        i2c_queue[queueIndex].pending = false;
        i2c_queue[queueIndex].rW = 0;
        i2c_queue[queueIndex].command = 0;
        i2c_queue[queueIndex].pcommandData = NULL;
        i2c_queue[queueIndex].commandDataSize = 0;
        i2c_queue[queueIndex].pData = NULL;
        i2c_queue[queueIndex].Size = 0;
        i2c_queue[queueIndex].pCallback = NULL;
    }
    
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_EN);
    /// Set low interrupt
    bit_low(I2C_INTERRUPT);
    
    /// Set available
    I2C_Busy = false;
}

/**
 * Reset the I2C module
 */
void I2C_reset(void) {
    I2C_state = &I2C_idle; // disable the response to any more interrupts
    
    I2C_ERROR = *I2C_STAT; // record the error for diagnostics
    
    REGISTER_MASK_SET_LOW(I2C_CON, MASK_I2CCON_EN);

    res_Callback(true);

    *I2C_CON = 0x1000;
    
    *I2C_STAT = 0x0000;
    
    I2C_load(); //< turn the I2C back on
    return;
}

bool I2C_checkACK(unsigned int command, I2C_callbackFunc pCallback) {
    if (!I2C_CheckAvailable()) return false;

    pI2C_callback = pCallback;

    I2C_command_data_size = 0;
    I2C_CommandByte = command;
    pI2CBuffer = NULL;
    
    I2C_data_size.tx = 0; // tx data size
    I2C_data_size.rx = 0; // rx data size

    // Set ISR callback and trigger the ISR
    I2C_state = &I2C_startWrite;
    /// Set high interrupt
    bit_high(I2C_INTERRUPT);
    return true;
}
/**
 * Send the message with additional data
 * @param command command data usually the address of peripherals
 * @param pcommandData additional message
 * @param commandDataSize size of additional message
 * @param rW type of message
 * @param ptrxData pointer to data
 * @param trxSize size of data
 * @param pCallback Callback when the controller complete or fail to send the message
 * @return status of sending message
 */
void I2C_loadCommand(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned short rW, unsigned char* ptrxData, unsigned int trxSize, I2C_callbackFunc pCallback) {
    
    pI2C_callback = pCallback;
    I2C_CommandByte = command;
    pI2CcommandBuffer = pcommandData;
    I2C_command_data_size = commandDataSize;
    pI2CBuffer = ptrxData;

    if (rW == I2C_COMMAND_WRITE) {
        I2C_data_size.tx = trxSize; // tx data size
        I2C_data_size.rx = 0; // rx data size
    } else {
        I2C_data_size.tx = 0; // tx data size
        I2C_data_size.rx = trxSize; // rx data size
    }

    // Set ISR callback and trigger the ISR
    I2C_state = &I2C_startWrite;
    /// Set high interrupt
    bit_high(I2C_INTERRUPT);
}
/**
 * Load in buffer the message with additional data
 * @param command command data usually the address of peripherals
 * @param pcommandData additional message
 * @param commandDataSize size of additional message
 * @param rW type of message
 * @param ptrxData pointer to data
 * @param trxSize size of data
 * @param pCallback Callback when the controller complete or fail to send the message
 * @return status of sending message
 */
i2c_state_t I2C_loadBuffer(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned short rW, unsigned char* ptrxData, unsigned int trxSize, I2C_callbackFunc pCallback) {
    int queueIndex;
    if (I2CMAXQ < I2C_QUEUE_DEPTH) {
        for (queueIndex = 0; queueIndex < I2C_QUEUE_DEPTH; queueIndex++) {
            if (i2c_queue[queueIndex].pending == false) {
                i2c_queue[queueIndex].pending = true;
                i2c_queue[queueIndex].rW = rW;
                i2c_queue[queueIndex].command = command;
                i2c_queue[queueIndex].pcommandData = pcommandData;
                i2c_queue[queueIndex].commandDataSize = commandDataSize;
                i2c_queue[queueIndex].pData = ptrxData;
                i2c_queue[queueIndex].Size = trxSize;
                i2c_queue[queueIndex].pCallback = pCallback;
                I2CMAXQ++;
                return PENDING;
            }
        }
    }
    return false;
}

i2c_state_t I2C_Write(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, I2C_callbackFunc pCallback) {
    return I2C_Write_data(command, pcommandData, commandDataSize, NULL, 0, pCallback);
}

i2c_state_t I2C_Write_data(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* ptxData, unsigned int txSize, I2C_callbackFunc pCallback) {
    
    // Try to direct send the message
    if(I2C_CheckAvailable() && (I2CMAXQ < I2C_QUEUE_DEPTH)) {
        I2C_loadCommand(command, pcommandData, commandDataSize, I2C_COMMAND_WRITE, ptxData, txSize, pCallback);
    } else {
        return I2C_loadBuffer(command, pcommandData, commandDataSize, I2C_COMMAND_WRITE, ptxData, txSize, pCallback);
    }
    return true;
}

i2c_state_t I2C_Read(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* prxData, unsigned int rxSize, I2C_callbackFunc pCallback) {
    
    // Try to direct send the message
    if(I2C_CheckAvailable() && (I2CMAXQ < I2C_QUEUE_DEPTH)) {
        I2C_loadCommand(command, pcommandData, commandDataSize, I2C_COMMAND_READ, prxData, rxSize, pCallback);
    } else {
        return I2C_loadBuffer(command, pcommandData, commandDataSize, I2C_COMMAND_READ, prxData, rxSize, pCallback);
    }
    return true;
}

/**
 * Serve all I2C messages in pending
 */
void I2C_serve_queue(void) {
    int queueIndex;
    for (queueIndex = 0; queueIndex < I2C_QUEUE_DEPTH; queueIndex++) {
        if (i2c_queue[queueIndex].pending == true) {
            // Send message to I2C
            I2C_loadCommand(i2c_queue[queueIndex].command, i2c_queue[queueIndex].pcommandData, i2c_queue[queueIndex].commandDataSize, 
                    i2c_queue[queueIndex].rW, i2c_queue[queueIndex].pData, i2c_queue[queueIndex].Size, i2c_queue[queueIndex].pCallback);
            //decrease queue
            I2CMAXQ--;
            i2c_queue[queueIndex].pending = false;
        }
    }
}
/**
 * Check the I2C is available
 * @return state of I2C
 */
inline bool I2C_CheckAvailable(void) {
    if (REGISTER_MASK_READ(I2C_CON, MASK_I2CCON_EN) == 0) return false;
    if (REGISTER_MASK_READ(I2C_STAT, 0b0000010011000000) != 0) return false;

    if (I2C_Busy == true) return false;
    I2C_Busy = true;

    return true;
}
/**
 * Enable write message in controller
 */
void I2C_startWrite(void) {
    I2C_Index = 0; // Reset index into buffer

    I2C_state = &I2C_writeCommand;
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_SEN);
    return;
}
/**
 * Write the command message (usually the address of peripherals
 */
void I2C_writeCommand(void) {
    *(I2C_TRN) = I2C_CommandByte & 0xFE;
    I2C_state = &I2C_writeCommandData;
    return;
}
/**
 * If the ACK return true send the command data and jump to read/write operation
 */
void I2C_writeCommandData(void) {
    if (REGISTER_MASK_READ(I2C_STAT, MASK_I2CSTAT_ACKSTAT) == 1) {
        // Device not responding
        I2C_Failed();
        return;
    }

    // If there is no command data, do not send any, do a stop.
    if (I2C_command_data_size == 0) {
        I2C_writeStop();
        return;
    }

    *(I2C_TRN) = pI2CcommandBuffer[I2C_Index++];

    if (I2C_Index >= I2C_command_data_size) {
        I2C_Index = 0; // Reset index into the buffer

        if (I2C_data_size.rx == I2C_COMMAND_READ)
            I2C_state = &I2C_readStart;
        else
            I2C_state = &I2C_writeData;
    }
    return;
}

/* READ FUNCTIONS */

/**
 * Start read operation
 */
void I2C_readStart(void) {
    I2C_Index = 0; // Reset index into buffer
    I2C_state = &I2C_readCommand;
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_SEN);
}
/**
 * Send write command message
 */
void I2C_readCommand(void) {
    I2C_state = &I2C_recen;
    *(I2C_TRN) = I2C_CommandByte | 0x01;
}
/**
 * Check the peripherals responding and start reading operation
 */
void I2C_recen(void) {
    if (REGISTER_MASK_READ(I2C_STAT, MASK_I2CSTAT_ACKSTAT) == 1) {
        // Device not responding
        I2C_Failed();
        return;
    } else {
        I2C_state = &I2C_recstore;
        REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_RCEN);
    }
    return;
}
/**
 * Store all read data in buffer
 */
void I2C_recstore(void) {
    if(pI2CBuffer != NULL) {
        pI2CBuffer[I2C_Index++] = *I2C_RCV;
        if (I2C_Index >= I2C_data_size.rx) {
            I2C_state = &I2C_stopRead;
            REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_ACKDT);
        } else {
            I2C_state = &I2C_rerecen;
            REGISTER_MASK_SET_LOW(I2C_CON, MASK_I2CCON_ACKDT);
        }
    }
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_ACKEN);
    return;
}
/**
 * Stop read
 */
void I2C_stopRead(void) {
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_PEN);
    I2C_state = &I2C_doneRead;
    return;
}
/**
 * Update read operation
 */
void I2C_rerecen(void) {
    I2C_state = &I2C_recstore;
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_RCEN);
    return;
}
/**
 * Done read and launch callback.
 * If the queue is not empty launch other message in queue
 */
void I2C_doneRead(void) {
    if (pI2C_callback != NULL)
        pI2C_callback(true);
    //Check I2C available and buffer not empty
    if(I2CMAXQ > 0)
        I2C_serve_queue();
    else 
        I2C_Busy = false;
}

/* WRITE FUNCTIONS */

/**
 * If the ACK return true send the message
 */
void I2C_writeData(void) {
    if (REGISTER_MASK_READ(I2C_STAT, MASK_I2CSTAT_ACKSTAT) == 1) {
        // Device not responding
        I2C_Failed();
        return;
    }

    if (I2C_data_size.tx == 0) {
        I2C_writeStop();
        return;
    }
    if (pI2CBuffer != NULL) {
        *(I2C_TRN) = pI2CBuffer[I2C_Index++];
        
        if (I2C_Index >= I2C_data_size.tx) {
            if (I2C_data_size.rx == 0)
                I2C_state = &I2C_writeStop;
            else
                I2C_state = &I2C_readStart;
        }
    }
    return;
}
/**
 * Launch stop operation
 */
void I2C_writeStop(void) {
    I2C_state = &I2C_doneWrite;
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_PEN);
    return;
}
/**
 * Done write and launch callback.
 * If the queue is not empty launch other message in queue
 */
void I2C_doneWrite(void) {
    if (pI2C_callback != NULL)
        pI2C_callback(true);
    //Check I2C available and buffer not empty
    if(I2CMAXQ > 0)
        I2C_serve_queue();
    else
        I2C_Busy = false;
}

/* SERVICE FUNCTIONS */

/**
 * Idle operation
 */
void I2C_idle(void) {
    return;
}
/**
 * Stop I2C read/write and launch callback with false
 */
void I2C_Failed(void) {
    I2C_state = &I2C_idle;
    REGISTER_MASK_SET_HIGH(I2C_CON, MASK_I2CCON_PEN);
    if (pI2C_callback != NULL)
        pI2C_callback(false);
    //Check I2C available and buffer not empty
    if(I2CMAXQ > 0)
        I2C_serve_queue();
    else
        I2C_Busy = false;
}
/**
 * Status of I2C
 * @return status I2C
 */
bool I2C_Normal(void) {
    if (REGISTER_MASK_READ(I2C_STAT, 0b0000010011000000) == 0)
        return true;
    else {
        I2C_ERROR = ((unsigned int) I2C_STAT);
        return false;
    }
}