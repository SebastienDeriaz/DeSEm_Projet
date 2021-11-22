#include "gpo.h"

namespace ep {

/**
 * @brief Construct a new Gpo object
 * 
 * @param p1 Pin number
 * @param port Port name
 */
Gpo::Gpo(GPIO_TypeDef * port, uint32_t pin) : Gpio(port, pin)
{

}

/**
 * @brief Turn output pin on
 * 
 */
void Gpo::on()
{
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}

/**
 * @brief Turn output pin off
 * 
 */
void Gpo::off()
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}

/**
 * @brief Set ouput pin state
 * 
 * @param state of the output pin 
 */
void Gpo::set(GPIO_PinState state)
{
	HAL_GPIO_WritePin(port, pin, state);
}

} // namespace ep
