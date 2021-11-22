#ifndef EP_GPI_H
#define EP_GPI_H

#include "gpio.h"

namespace ep {

/**
 * @class General Purpose Input
 * @brief This class implements the attributes and methods of a General Purpose Input
 */
class Gpi : public Gpio
{
public:
    Gpi(GPIO_TypeDef* port, uint32_t pin);
    GPIO_PinState read();

};

} // namespace ep
#endif // EP_GPI_H
