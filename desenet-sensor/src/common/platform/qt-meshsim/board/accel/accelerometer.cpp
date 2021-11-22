#include <assert.h>
#include <random>
#include <algorithm>
#include <QtCore/QRect>
#include <QtWidgets/QApplication>
#include <QScreen>
#include <QtWidgets/QWidget>
#include "platform-config.h"
#include "trace/trace.h"
#include "xf/xf.h"
#include "accelerometer.h"

namespace board {

QWidget * Accelerometer::_pMainWindow(nullptr);

Accelerometer::Accelerometer()
{

}

bool Accelerometer::initialize()
{
    if (_pMainWindow == NULL)
    {
        QWidgetList widgets = QApplication::allWidgets();
        if (widgets.size() > 0)
        {
            _pMainWindow = widgets[0];
        }
    }
    return true;
}

// static
void Accelerometer::getAccelerationValues(int16_t & x, int16_t & y, int16_t & z)
{
    //
    // The accelerometer minimum value is -2048 and the maximum value is +2048
    //
    QRect desktopRect = QGuiApplication::screens().first()->availableGeometry();    // Desktop rectangle
    QRect widgetRect = _pMainWindow->geometry();                                    // Main window rectangle

    // Point for calculations is the center point of the windows title
    QPoint pointOfInterest(widgetRect.topLeft().x() + widgetRect.width()/2,
                           widgetRect.topLeft().y());

    const float slopeX = 4096.0 / desktopRect.size().width();
    const float slopeY = 4096.0 / desktopRect.size().height();

    x = slopeX * abs(pointOfInterest.x()) - 2048;		// Calculate x
    y = slopeY * abs(pointOfInterest.y()) - 2048;		// Calculate y

    // Bring back to valid range
    x = std::max((int32_t)x, -2048); x = std::min((int32_t)x, 2048);
    y = std::max((int32_t)y, -2048); y = std::min((int32_t)y, 2048);

    // z is a mix of x and y
    z = (x + y) / 2;
}

} // namespace board
