#include <stddef.h>
#include <assert.h>
#include "mcu/mcu.h"
#include "spi.h"

extern "C" SPI_HandleTypeDef hspi2;     // All defined in main.c
extern "C" SPI_HandleTypeDef hspi3;

Spi::Spi(int spiNbr)
 : _bInitialized(false),
   _pSPI(NULL)
{
	switch (spiNbr)
	{
	case 1:
		_pSPI = &hspi2;
		break;
	case 2:
		_pSPI = &hspi3;
		break;
	default:
		assert(false);	// Passed wrong parameter
	}
}

Spi::~Spi()
{

}

Spi::Spi(const Spi &)
{
}

//static
Spi & Spi::instance(uint8_t spiIndex)
{
	Spi * pSpi = NULL;

	switch (spiIndex)
	{
	case 0:
		{
			static Spi spi1(0);

			pSpi = &spi1;
		}
		break;
	case 1:
		{
			static Spi spi2(1);

			pSpi = &spi2;
		}
		break;
	case 2:
		{
			static Spi spi3(2);

			pSpi = &spi3;
		}
		break;
	default:
		break;
	}

	if (!pSpi)
	{
		assert(false);
	}

	return *pSpi;
}

/**
 * Partially initializes the SPI. Other part of the initialization
 * needs to be made in an other place (see in board configuration).
 *
 * After complete initialization the SPI can be enabled using the #enable method.
 */
bool Spi::initialize()
{
	if (!_bInitialized)
	{
		_bInitialized = true;

#ifdef IDE_STM32CUBEIDE
		// HAL initialization is done in the main.c file
		return true;
#endif // IDE_STM32CUBEIDE

		return true;
	}

	return false;
}

void Spi::enable()
{

}

void Spi::disable()
{

}

/**
 * Resets the state machine of the SPI. Useful only
 * when accessing multiple chips with one SPI
 */
void Spi::resetStateMachine()
{
	// Reset state machine of the SPI by resetting the periphery.
	HAL_SPI_DeInit(_pSPI);
}

//bool Spi::getFlagStatus(unsigned short flagMask)
//{
//	assert(_pSPI);
//	return SPI_I2S_GetFlagStatus(_pSPI, flagMask);
//}

Spi::tStatus Spi::sendByte(unsigned char data)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint8_t rxData[1];

	status = HAL_SPI_TransmitReceive(_pSPI, &data, rxData, 1, 50);
	assert(status == HAL_OK);

	// Return the byte read from the SPI bus
	return rxData[0];
}
