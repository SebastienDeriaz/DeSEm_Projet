#include "gui/MonochromeDisplayPainter"
#include "simpledisplay.h"

SimpleDisplay::SimpleDisplay()
{
}

SimpleDisplay::~SimpleDisplay()
{
}

bool SimpleDisplay::initialize()
{
	return BaseClass::initialize();
}

void SimpleDisplay::clear()
{
	MonochromeDisplayPainter painter(this, false);
	painter.clear();
	painter.end();
}

void SimpleDisplay::clearRow(uint8_t row)
{
	MonochromeDisplayPainter painter(this, false);
	painter.begin(this);
	painter.drawRectangle(Point(0, row * 8), Size(SimpleDisplay::X_MAX, 8), IMonochromeDisplay::White, IMonochromeDisplay::White);
	painter.end();
}

void SimpleDisplay::drawText(const char * str, uint16_t col, uint16_t row)
{
	MonochromeDisplayPainter painter(this, false);
	painter.begin(this);
	painter.drawText(IMonochromeDisplay::Point(col, row*8), str, IMonochromeDisplay::Black);
	painter.end();
}

void SimpleDisplay::drawLine(const SimpleDisplay::Point & begin, const SimpleDisplay::Point & end)
{
	MonochromeDisplayPainter painter(this, false);
	painter.begin(this);
	painter.drawLine(begin, end, IMonochromeDisplay::Black);
	painter.end();
}

void SimpleDisplay::drawRectangle(const SimpleDisplay::Point & topLeft, const SimpleDisplay::Size & size, bool fill)
{
	MonochromeDisplayPainter painter(this, false);
	painter.begin(this);
	painter.drawRectangle(topLeft, size, IMonochromeDisplay::Black, (fill) ? IMonochromeDisplay::Black : IMonochromeDisplay::Undefined);
	painter.end();
}

