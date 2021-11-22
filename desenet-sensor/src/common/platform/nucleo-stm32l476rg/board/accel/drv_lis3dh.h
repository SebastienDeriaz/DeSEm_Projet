/********************************************************************************/
/**
 * \file
 *
 * \brief Library offering LIS3DH functions 
 *        for accelerometer sensor
 *
 * \version 1.0 (original)
 *
 *
 * \author pascal.sartoretti@hevs.ch
 *
 */
/********************************************************************************/

/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __accelerometer_H
#define __accelerometer_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include "mcu/mcu.h"

#define ACCEL_RESPONSE_ERROR			0xFFFF0001	///< who am I don't answer


/********************************************************************************/
/**
 * \brief  Initialise the accelerometer with 25Hz, high resolution
 * \return Returns any error or 0 if all was ok.
 */
/********************************************************************************/
uint32_t Accelerometer_Init(void);
/********************************************************************************/
/**  
 * \brief  Get the result of x,y,z accelerometer values
 * \param x pointer to signed 16 bits mg x value
 * \param y pointer to signed 16 bits mg y value
 * \param z pointer to signed 16 bits mg z value
 * \return Returns any error or 0 if all was ok.
 */
/********************************************************************************/
uint32_t Accelerometer_Get(int16_t * x, int16_t * y, int16_t * z);
/********************************************************************************/
/**  
 * \brief  Get interrupt status and clear it
 * \return  Interrupt status
 */
/********************************************************************************/
bool Accelerometer_IsInterruptFired();
/********************************************************************************/
/**  
 * \brief  Clear accelerometer interrupt
 * \return  Interrupt status
 */
/********************************************************************************/
bool Accelerometer_ClearInterrupt();
/********************************************************************************/
/**  
 * \brief  Enable accelerometer interrupts
 */
/********************************************************************************/
void Accelerometer_EnableInterrupt();
/********************************************************************************/
/**  
 * \brief  Power down the accelerometer and stop measures (500nA vs 6uA)
 * Don't use Accelerometer_Get after this function call.
 * To restart measures, call Accelerometer_Init
 */
/********************************************************************************/
void Accelerometer_PowerDown();

#ifdef __cplusplus
}
#endif
#endif /*__ sensors_H */

