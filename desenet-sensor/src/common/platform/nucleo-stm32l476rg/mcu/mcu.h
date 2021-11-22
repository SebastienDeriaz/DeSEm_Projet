#ifndef MCU_H
#define MCU_H

#include "stm32l4xx_hal.h"

#ifdef __cplusplus

class Mcu
{
public:
	inline static void sleep()
	{
#if (MCU_SLEEP_ON_IDLE != 0)
		__WFE();	// Wait as long as no event at MCU level occurs (Wait For Event).
#endif
	};
};

#endif // __cplusplus
#endif // MCU_H
