#ifndef BOARD_ACCELEROMETER_H
#define BOARD_ACCELEROMETER_H

#include <cstdint>

namespace board {

/**
 * @brief Class interfacing the real accelerometer hardware.
 */
class Accelerometer
{
public:
	Accelerometer() {}
	~Accelerometer() {}

	bool initialize();

	static void getAccelerationValues(int16_t & x, int16_t & y, int16_t & z);
	static void onIrq(uint16_t GPIO_Pin);

protected:
	static void initializeAccelerometer();
	inline static bool isInitialized() { return _bInitialized; }

    static void suspend();
    static void wakeup();

private:
    static bool _bInitialized;
};

} // namespace board

#endif // BOARD_ACCELEROMETER_H
