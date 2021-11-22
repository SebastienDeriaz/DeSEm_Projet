#include <cassert>
#include "platform-config.h"
#include "trace/trace.h"
#include "xf/xf.h"
#include "drv_lis3dh.h"
#include "accelerometer.h"

namespace board {

#define NSAMPLE  1

typedef union
{
	unsigned char data[2];
	signed short result;
} acc_data;

bool Accelerometer::_bInitialized = false;

void Accelerometer::initializeAccelerometer()
{
    Accelerometer_Init();

#if (ACCELEROMETER_ENABLE_IRQ != 0)
    Accelerometer_EnableInterrupt();
#endif
}

// static
void Accelerometer::getAccelerationValues(int16_t & x, int16_t & y, int16_t & z)
{
	int16_t tx = 0, ty = 0, tz = 0;

	Accelerometer_Get(&tx, &ty, &tz);

	x = tx;
	y = ty;
	z = tz;
}

void Accelerometer::onIrq(uint16_t GPIO_Pin)
{
#if (ACCELEROMETER_VERBOSE != 0)
	Trace::outln("Acc::FF_WU_SRC");
#endif
#if (ACCELEROMETER_ENABLE_IRQ != 0)
	Accelerometer_ClearInterrupt();
#endif
}

bool Accelerometer::initialize()
{
	if (!isInitialized())
	{
		initializeAccelerometer();
		return true;
	}
	assert(false);	// Resource should only be initialized once. Maybe you have more then one instance of Accelerometer.
	return false;
}

// static
void Accelerometer::suspend()
{
#if (ACCELEROMETER_VERBOSE != 0)
	Trace::out("Acc::suspend");
#endif
	Accelerometer_PowerDown();
}

// static
void Accelerometer::wakeup()
{
#if (ACCELEROMETER_VERBOSE != 0)
	Trace::out("Acc::wakeup");
#endif
	Accelerometer_Init();
}

} // namespace board
