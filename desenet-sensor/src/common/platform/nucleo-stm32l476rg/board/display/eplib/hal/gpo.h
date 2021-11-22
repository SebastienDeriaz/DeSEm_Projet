#ifndef EP_GPO_H
#define EP_GPO_H

#include "gpio.h"

namespace ep {

/**
 * @class General Purpose Output 
 * @brief This class implements the attributes of a General Purpose Output
 */
class Gpo : public Gpio
{
public:

    Gpo(GPIO_TypeDef * port, uint32_t pin);
    void on();
    void off();
    void set(GPIO_PinState state);
};

} // namespace ep
#endif // EP_GPO_H
