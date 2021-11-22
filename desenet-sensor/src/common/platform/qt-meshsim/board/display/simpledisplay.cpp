#include "gui/MonochromeDisplayPainter"
#include "meshsimboard.h"
#include "app/factory.h"
#include "simpledisplay.h"

SimpleDisplay::SimpleDisplay()
{
}

SimpleDisplay::~SimpleDisplay()
{
}

bool SimpleDisplay::initialize()
{
    return meshSimBoard().initialize();
}

MeshSimBoard & SimpleDisplay::meshSimBoard() const
{
    return app::Factory::instance().meshSimBoard();
}

void SimpleDisplay::clear()
{
    MonochromeDisplayPainter painter(&meshSimBoard(), false);
    painter.clear();
    painter.end();
}

void SimpleDisplay::clearRow(uint8_t row)
{
    MonochromeDisplayPainter painter(&meshSimBoard(), false);
    painter.begin(&meshSimBoard());
    painter.drawRectangle(Point(0, row * 8), Size(SimpleDisplay::X_MAX, 8), IMonochromeDisplay::White, IMonochromeDisplay::White);
    painter.end();
}

void SimpleDisplay::drawText(const char * str, uint16_t col, uint16_t row)
{
    MonochromeDisplayPainter painter(&meshSimBoard(), false);
    painter.begin(&meshSimBoard());
    painter.drawText(IMonochromeDisplay::Point(col, row*8), str, IMonochromeDisplay::Black);
    painter.end();
}

void SimpleDisplay::drawLine(const SimpleDisplay::Point & begin, const SimpleDisplay::Point & end)
{
    MonochromeDisplayPainter painter(&meshSimBoard(), false);
    painter.begin(&meshSimBoard());
    painter.drawLine(begin, end, IMonochromeDisplay::Black);
    painter.end();
}

void SimpleDisplay::drawRectangle(const SimpleDisplay::Point & topLeft, const SimpleDisplay::Size & size, bool fill)
{
    MonochromeDisplayPainter painter(&meshSimBoard(), false);
    painter.begin(&meshSimBoard());
    painter.drawRectangle(topLeft, size, IMonochromeDisplay::Black, (fill) ? IMonochromeDisplay::Black : IMonochromeDisplay::Undefined);
    painter.end();
}

