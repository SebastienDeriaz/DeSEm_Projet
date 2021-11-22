#include <cmath>
#include "mcu/mcu.h"
#include "main.h"
#include "trace/trace.h"
#include "monochromedisplay.h"

extern "C" SPI_HandleTypeDef hspi2;         // Defined in main.c

MonochromeDisplay::MonochromeDisplay() :
 pinDC(DISP_DATA_GPIO_Port, DISP_DATA_Pin),
 pinBusy(DISP_BUSY_GPIO_Port, DISP_BUSY_Pin),
 pinReset(DISP_RESET_GPIO_Port, DISP_RESET_Pin)
{
    rotation = EP_DISPLAY_ROTATE_0;
}

MonochromeDisplay::~MonochromeDisplay()
{

}

IMonochromeDisplay::Size MonochromeDisplay::resolution() const
{
	return Size(Width, Height);
}

IMonochromeDisplay::AcceleratedDrawingMethods MonochromeDisplay::acceleratedDrawingMethods() const
{
	AcceleratedDrawingMethods accels;

	memset(&accels, 0, sizeof(AcceleratedDrawingMethods));

	accels.clearAccelerated = 1;
	accels.drawLineAccelerated = 1;
	accels.drawTextAccelerated = 1;

	return accels;
}

bool MonochromeDisplay::initialize()
{
    rotation = EP_DISPLAY_ROTATE_270;   // Turn by 270° for the DeSEm Nucleo STM32L476 hardware

    display.initialize(&hspi2, &pinDC, &pinBusy, &pinReset);
    painter.initialize(&display);

    painter.setRotate(rotation);

    painter.setBuffer(_lcdBuffer, Width, Height, rotation, EP_DISPLAY_WHITE);

	clear();                            // Clear ePaper display (all pixels to white)
	display.setLUTShortRefresh();       // Change to fast refresh

	return true;
}

bool MonochromeDisplay::enable()
{
	return true;    // ePaper display always on
}

bool MonochromeDisplay::disable()
{
	return true;    // ePaper display always on
}

IMonochromeDisplay::PixelColor MonochromeDisplay::pixel(const Point & point)
{
    // TODO: Returning right pixel is not easy for the ePaper display
    //       because internally the display can be rotated and even mirrored
    // painter.pixel(point);
    return White;
}

void MonochromeDisplay::setPixel(const Point & point , PixelColor color)
{
    painter.setPixel(point.x, point.y, toEpDisplayColor(color), false);
}

void MonochromeDisplay::clear(PixelColor background /* = White */)
{
	painter.clear(toEpDisplayColor(background));
}

void MonochromeDisplay::drawLine(const Point & begin, const Point & end, PixelColor color)
{
    painter.drawLine(begin.x, begin.y, end.x, end.y, toEpDisplayColor(color), LINE_STYLE_SOLID, DOT_PIXEL_2X2, false);
    painter.update();
}

void MonochromeDisplay::drawText(const Point & topLeft , const std::string & text, PixelColor color)
{
    painter.drawString(topLeft.x, topLeft.y, text.c_str(), &Font24, EP_DISPLAY_WHITE, toEpDisplayColor(color), true);
}

void MonochromeDisplay::sendCommand(unsigned char cmd)
{
	display.writeCMD(cmd);
}

void MonochromeDisplay::sendData(unsigned char data)
{
	display.writeDATA(data);
}

void MonochromeDisplay::update()
{
	painter.update();
}

uint16_t MonochromeDisplay::toEpDisplayColor(PixelColor pixelColor)
{
    return (pixelColor == IMonochromeDisplay::White) ? EP_DISPLAY_WHITE : EP_DISPLAY_BLACK;
}
