/* 
 * File:   i2c.h
 * Author: Raffaello
 *
 * Created on June 23, 2015, 10:51 AM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    #include <system/events.h>

/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
    /// State of I2C command
    typedef enum {
        FALSE = 0,      ///< I2C can't send the message
        TRUE = 1,       ///< I2C send the message
        PENDING = 2     ///< Message in pending
    } i2c_state_t;
    
    /// callback type for I2C user
    typedef void (*I2C_callbackFunc)(bool);
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

    /**
     * Initialize the I2C peripheral and trigger the I2C service routine to run
     * @param i2c_interrupt I2C interrupt line
     * @param i2c_con I2C configuration register
     * @param i2c_stat I2C status register
     * @param i2c_trn I2C transmission register
     * @param i2c_rcv I2C reception register
     * @param resetCallback additional operation when reset I2C
     * @return Number event
     */
    hEvent_t I2C_Init(hardware_bit_t* i2c_interrupt, REGISTER i2c_con, REGISTER i2c_stat, REGISTER i2c_trn, REGISTER i2c_rcv, I2C_callbackFunc resetCallback);
    
    /**
     * Check for I2C ACK on command
     * @param command command data usually the address of peripherals
     * @param pCallback callback received
     * @return state of I2C
     */
    bool I2C_checkACK(unsigned int command, I2C_callbackFunc pCallback);

    /**
     * Write command without additional data
     * @param command command data usually the address of peripherals
     * @param pcommandData additional message
     * @param commandDataSize size of additional message
     * @param pCallback Callback when the controller complete or fail to send the message
     * @return status of sending message
     */
    i2c_state_t I2C_Write(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, I2C_callbackFunc pCallback);
    
    /**
     * Write a message with additional data
     * @param command command data usually the address of peripherals
     * @param pcommandData additional message
     * @param commandDataSize size of additional message
     * @param ptxData pointer to transmission data
     * @param txSize size of data
     * @param pCallback Callback when the controller complete or fail to send the message
     * @return status of sending message
     */
    i2c_state_t I2C_Write_data(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* ptxData, unsigned int txSize, I2C_callbackFunc pCallback);
    
    /**
     * 
     * @param command command data usually the address of peripherals
     * @param pcommandData additional message
     * @param commandDataSize size of additional message
     * @param prxData pointer to received data
     * @param rxSize size of data
     * @param pCallback Callback when the controller complete or fail to send the message
     * @return status of sending message
     */
    i2c_state_t I2C_Read(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* prxData, unsigned int rxSize, I2C_callbackFunc pCallback);

    /**
     * This function you must add in I2C interrupt
     */
    inline void I2C_manager (void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

