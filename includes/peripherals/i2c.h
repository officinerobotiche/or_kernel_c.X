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

    /// callback type for I2C user
    typedef void (*I2C_callbackFunc)(bool);
    
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

    /**
     * Trigger the I2C service routine to run at low priority in libUDB.c
     * initialize the I2C peripheral
     */
    void I2C_Init(hardware_bit_t* i2c_interrupt, REGISTER i2c_con, REGISTER i2c_stat, REGISTER i2c_trn, REGISTER i2c_rcv);
    /**
     * Reset the I2C module
     */
    void I2C_reset(void);
    /**
     * Check for I2C ACK on command
     * @param command
     * @param pCallback
     * @return 
     */
    bool I2C_checkACK(unsigned int command, I2C_callbackFunc pCallback);

    /**
     * 
     * @param command
     * @param pcommandData
     * @param commandDataSize
     * @param pCallback
     * @return 
     */
    bool I2C_Write(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, I2C_callbackFunc pCallback);
    
    /**
     * 
     * @param command
     * @param pcommandData
     * @param commandDataSize
     * @param ptxData
     * @param txSize
     * @param pCallback
     * @return 
     */
    bool I2C_Write_data(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* ptxData, unsigned int txSize, I2C_callbackFunc pCallback);
    
    /**
     * 
     * @param command
     * @param pcommandData
     * @param commandDataSize
     * @param prxData
     * @param rxSize
     * @param pCallback
     * @return 
     */
    bool I2C_Read(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* prxData, unsigned int rxSize, I2C_callbackFunc pCallback);

    /**
     * 
     */
    inline void I2C_manager (void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

