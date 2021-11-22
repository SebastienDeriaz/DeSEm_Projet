#ifndef BOARD_SIMPLEDISPLAY_H
#define BOARD_SIMPLEDISPLAY_H

#include "board/display/monochromedisplay.h"

/**
 * @brief Easy access to LCD display
 *
 * This class provides the possibility to write text to a display and
 * to draw lines and rectangles on it. In case you want draw more
 * complex staff please refer to the MonochromDisplay and
 * the MonochromeDisplayPainter class.
 */
class SimpleDisplay : protected MonochromeDisplay
{
public:
	typedef IMonochromeDisplay::Point Point;
	typedef IMonochromeDisplay::Size Size;

	static const uint8_t X_MAX = MonochromeDisplay::Width - 1;											///< Display's maximum x value
	static const uint8_t Y_MAX = MonochromeDisplay::Height - 1;											///< Display's maximum y value
	static const uint8_t ROW_MAX = MonochromeDisplay::Width - 1;										///< Display's maximum row value

public:
	SimpleDisplay();
	virtual ~SimpleDisplay();

	bool initialize();		///< Initializes the simple display.

	void clear();																	///< Clears entire content of the display.
	void clearRow(uint8_t row);														///< Clears content of one row.
	void drawText(const char * str, uint16_t col, uint16_t row);					///< Draws text at given column and row.
	void drawLine(const Point & begin, const Point & end);							///< Draws line from point to point.
	void drawRectangle(const Point & topLeft, const Size & size, bool fill);		///< Draws a rectangle at specified coordinates.

protected:
	typedef MonochromeDisplay BaseClass;
};

#endif // BOARD_SIMPLEDISPLAY_H
