#include "mcu/mcu.h"
#include "gpi.h"

namespace ep {

/**
 * @brief Initialize a new GPI object 
 * 
 * @param p1 Pin Number
 * @param port Port Name
 */
Gpi::Gpi(GPIO_TypeDef * port, uint32_t pin) : Gpio(port, pin)
{

}

/**
 * @brief Read current pin state 
 * 
 * @return GPIO::PINSTATE (PIN_ON/PIN_OFF)
 */
GPIO_PinState Gpi::read()
{
	GPIO_PinState retVal = HAL_GPIO_ReadPin(port, pin);
    
    return retVal;
}

} // namespace ep
