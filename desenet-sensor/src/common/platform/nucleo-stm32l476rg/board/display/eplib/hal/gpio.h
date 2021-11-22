#ifndef EP_GPIO_H
#define EP_GPIO_H

#include <cstdint>
#include "mcu/mcu.h"

namespace ep {

/**
 * @class General Purpose Input/Output
 * @brief This class implements the Attributes and Methods of a General Purpose Input/Output.
 */
class Gpio
{
public:
    Gpio(GPIO_TypeDef * port, uint32_t pin);

protected:
    GPIO_TypeDef* port;         ///< GPIO port identifier.
    uint32_t pin;               ///< GPIO pin number.
};

} // namespace ep
#endif // EP_GPIO_H
