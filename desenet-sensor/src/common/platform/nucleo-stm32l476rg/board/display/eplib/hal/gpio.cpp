#include "gpio.h"

namespace ep {

/**
 * @brief Construct a new GPIO object 
 * 
 * @param p1 Pin Number
 * @param port Port Name
 */
Gpio::Gpio(GPIO_TypeDef * port, uint32_t pin) :
 port(port),
 pin(pin)
{

}

} // namespace ep
