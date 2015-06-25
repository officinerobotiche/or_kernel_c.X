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
    void I2C_Init(void);
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
     * @param ptxData
     * @param txSize
     * @param pCallback
     * @return 
     */
    bool I2C_Write(unsigned char command, unsigned char* ptxData, unsigned int txSize, I2C_callbackFunc pCallback);
    /**
     * 
     * @param command
     * @param prxData
     * @param rxSize
     * @param pCallback
     * @return 
     */
    bool I2C_Read(unsigned char command, unsigned char* prxData, unsigned int rxSize, I2C_callbackFunc pCallback);



#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

