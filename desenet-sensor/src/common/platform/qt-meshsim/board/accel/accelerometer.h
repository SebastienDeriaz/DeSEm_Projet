#pragma once

#include <stdint.h>

class QWidget;

namespace board {

/**
 * @brief Class interfacing the real accelerometer hardware.
 */
class Accelerometer
{
public:
	Accelerometer();
	~Accelerometer() {}

	bool initialize();
	static void getAccelerationValues(int16_t & x, int16_t & y, int16_t & z);

protected:
	inline static bool isInitialized() { return true; }

protected:
    static QWidget * _pMainWindow;
};

} // namespace board
