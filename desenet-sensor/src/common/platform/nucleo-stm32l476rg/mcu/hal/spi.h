#ifndef HAL_SPI_H
#define HAL_SPI_H

#include "mcu/mcu.h"

/**
 * @brief Provides general access to the microcontrollers SPI peripheral.
 */
class Spi
{
	friend class Factory;

public:
	typedef unsigned char tStatus;

	static Spi & instance(uint8_t spiIndex);
	bool initialize();

	void enable();
	void disable();
	void resetStateMachine();

//	bool getFlagStatus(unsigned short flagMask);

	tStatus sendByte(unsigned char data);

protected:
	/**
	 * Constructor of the class. Only Factory class is
	 * allowed to create instances of this class
	 *
	 * \param spiNbr The SPI to be accessed. Valid numbers are 0 to 2
	 */
	Spi(int spiNbr);	// Only Factory class is allowed to create instances of this class
	virtual ~Spi();

	Spi(const Spi &);														///< Forbids to copy an object.
	const Spi & operator =(const Spi & spi) { (void)spi; return spi; }		///< Forbids to copy an application.


protected:
	bool _bInitialized;
	SPI_HandleTypeDef * _pSPI;              ///< Reference to the SPI structure
};

#endif // HAL_SPI_H
