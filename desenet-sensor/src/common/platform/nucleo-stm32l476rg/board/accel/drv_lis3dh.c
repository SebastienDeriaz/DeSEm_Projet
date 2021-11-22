/**
 * \file
 *
 * \brief Library offering LIS3DH functions
 *        for accelerometer
 *
 * \version 1.0 (original)
 *
 *
 * \author pascal.sartoretti@hevs.ch
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include "mcu/mcu.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c3;     // Defined in main.c

/// \defgroup g_accel Accelerometer
/// @{
//--------------------------------------------------------------------------------
// accelerometer definitions
//--------------------------------------------------------------------------------
#define ACCEL_I2C                           hi2c3
#define ACCEL_ADDR	                        (0x19 << 1)	    ///< accelerometer I2C address
#define ACCEL_WHO_AM_I						0x0F			///< who am I address
#define ACCEL_WHO_AM_I_DATA				    0x33			///< who am I data
#define ACCEL_CTRL_REG0						0x1E			///< 0x10 - enable pu, 0x90 - disable pu
#define ACCEL_CTRL_REG1						0x20			///< data rate selection, axis enable
#define ACCEL_CTRL_REG3						0x22
#define ACCEL_CTRL_REG5						0x24
#define ACCEL_STATUS_REG					0x27
#define ACCEL_DATA_REGS						0xA8			///< data addr 0x28 to 0x2D x,y,z (L,H) (auto increment)
#define ACCEL_INT1_CFG            0x30      ///< Interrupt 1 configuration
#define ACCEL_INT1_SRC            0x31      ///< Interrupt 1 source register (read only)
#define ACCEL_INT1_THS            0x32
#define ACCEL_INT1_DURATION       0x33

#define ACCEL_RESPONSE_ERROR			0xFFFF0001	///< who am I don't answer


//********************************************************************************
// Init this sensors
//********************************************************************************
uint32_t Accelerometer_Init()
{
    uint8_t data[5];

    data[0] = ACCEL_WHO_AM_I;
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,1,20);
    HAL_I2C_Master_Receive(&ACCEL_I2C,ACCEL_ADDR,data,1,20);
    if(data[0] != ACCEL_WHO_AM_I_DATA)
    {
        return ACCEL_RESPONSE_ERROR;
    }
    //------------------------------------------------------------------------------
    data[0] = ACCEL_CTRL_REG0;						// disable internal pull-up
    data[1] = 0x90;												// 25 Hz measure
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);
    //------------------------------------------------------------------------------
    data[0] = ACCEL_CTRL_REG1;
    data[1] = 0xB7;												// 25 Hz measure, high resolution
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);

    return 0;   // no error
}

//********************************************************************************
// Measure this sensors values x,y,z
// Values are signed in mg
//********************************************************************************
uint32_t Accelerometer_Get(int16_t * x, int16_t * y, int16_t * z)
{
    uint8_t tmp[7];

    tmp[0] = ACCEL_DATA_REGS;
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,tmp,1,20);
    HAL_I2C_Master_Receive(&ACCEL_I2C,ACCEL_ADDR,tmp,6,20);
    *x = tmp[0] | (tmp[1] << 8);
    *y = tmp[2] | (tmp[3] << 8);
    *z = tmp[4] | (tmp[5] << 8);
    // for high resolution (shift value)
    *x = *x >> 4;
    *y = *y >> 4;
    *z = *z >> 4;
    return 0;
}

//********************************************************************************
// Clear ISR
//********************************************************************************
bool Accelerometer_IsInterruptFired()
{
    uint8_t data[5];
    data[0] = ACCEL_INT1_SRC;

    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,1,20);
    HAL_I2C_Master_Receive(&ACCEL_I2C,ACCEL_ADDR,data,1,20);

    return (data[0] != 0);
}

//********************************************************************************
// Clear ISR
//********************************************************************************
bool Accelerometer_ClearInterrupt()
{
    return Accelerometer_IsInterruptFired();
}

//********************************************************************************
// Enable interrupt
//********************************************************************************
void Accelerometer_EnableInterrupt()
{
    uint8_t data[5];

    data[0] = ACCEL_CTRL_REG3;
    data[1] = 0x40;     // Enable I1_AOI1 interrupt
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);

    data[0] = ACCEL_CTRL_REG5;
    //data[1] = 0x08;     // Enable LIR_INT1 latched
    data[1] = 0x00;
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);

    data[0] = ACCEL_INT1_CFG;
    data[1] = 0x00;     // Disable interrupts on all axes and all directions
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);

    Accelerometer_ClearInterrupt();    // Clear previous interrupts

    data[0] = ACCEL_INT1_THS;
    data[1] = 0x03;     // Set interrupt threshold: 3 (1 and 2 not working!)
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);

    data[0] = ACCEL_INT1_DURATION;
    data[1] = 0x0A;     // Set event duration: 2 or higher
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);

    data[0] = ACCEL_INT1_CFG;
    data[1] = 0xAA;     // Detect 'move' on all axes
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);
}

//********************************************************************************
// Power down accelerometer
//********************************************************************************
void Accelerometer_PowerDown()
{
    uint8_t data[2];

    data[0] = ACCEL_CTRL_REG1;
    data[1] = 0x00;     // powerdown mode
    HAL_I2C_Master_Transmit(&ACCEL_I2C,ACCEL_ADDR,data,2,20);
}
/// @}

