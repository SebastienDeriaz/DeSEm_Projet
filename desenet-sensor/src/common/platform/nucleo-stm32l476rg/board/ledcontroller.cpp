#include <cassert>
#include "main.h"
#include "mcu/mcu.h"
#include "ledcontroller.h"

LedController::LedController()
 : _bInitialized(false),
   _ledFlasher(0, *this)
{

}

LedController::~LedController()
{

}

//static
LedController & LedController::instance()
{
	static LedController ledController;
	
	return ledController;
}

bool LedController::initialize()
{
	if (!_bInitialized)
	{
		_bInitialized = true;

	}
	return true;
}

void LedController::start()
{
	_ledFlasher.start();
}

void LedController::setLed(int index, bool bEnable)
{
	assert(_bInitialized);	// Call method initialize() first!

	if (index == 0)
	{
		// Using green LED 'LD2' on Nucleo board
		if (bEnable)
		{

			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
		}
		else
		{
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
		}
	}
}

void LedController::flashLed(uint16_t index /* = 0 */)
{
	if (index == 0)
	{
		_ledFlasher.flashLed();
	}
}

const LedController & LedController::operator =(const LedController & j)
{
    UNUSED(j);
    return j;
}
