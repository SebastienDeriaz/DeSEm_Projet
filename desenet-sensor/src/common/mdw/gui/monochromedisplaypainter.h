#pragma once
#include <cstdlib>
#include <stdint.h>
#include <string>
#include "platform-config.h"
#include "interfaces/imonochromedisplay.h"

/**
 * @ingroup utils
 * @brief This class enables to paint the same manner into monochrome displays with or without graphical
 * drawing accelerations exactly the same way.
 *
 * The usage is quite simple:
 * @code
 * MonochromeDisplayPainter painter( &display );
 *
 * painter.drawEllipse( IMonochromeDisplay::Point( 0 , 0 ) , IMonochromeDisplay::Size( 10 , 10 )
 *                      IMonochromeDisplay::Black );
 * @endcode
 */
class MonochromeDisplayPainter
{
public:
	/**
	 * @brief Creates a new instance of the painter class that works on the monochrome display interface given by
	 * 		  the pointer display.
	 *
	 * @param display 		Pointer to the object implementing the IMonochromeDisplay interface to use for drawing.
	 * @param autoUpdate 	If true, the display is automatically updated after each drawing access, otherwise only
	 * 					 	if the update method was explicitly called.
	 */
	explicit MonochromeDisplayPainter( IMonochromeDisplay * display = NULL , bool autoUpdate = true );

	/**
	 * @brief Destructor, releases dynamically allocated elements.
	 */
	virtual ~MonochromeDisplayPainter();

	/**
	 * @brief If the painter has to work on another display, the new display to work on can be set with this method.
	 *
	 * @param display The display to work on.
	 */
	void begin( IMonochromeDisplay * display );

	/**
	 * @brief If drawing with the current display has been finished, call this method or the destructor will do the
	 * 		  same.
	 */
	void end();

	/**
	 * @brief Clears the complete screen of the display with the given background color.
	 *
	 * @param background The pixel color to use for the background.
	 */
	void clear( IMonochromeDisplay::PixelColor background = IMonochromeDisplay::White );

	/**
	 * @brief Returns true if the auto update feature is enabled, false otherwise.
	 *
	 * @return True if auto update enabled, false otherwise.
	 */
	bool autoUpdate() const;

	/**
	 * @brief Sets the auto update functions to either enabled (true) or disabled (false).
	 *
	 * @param autoUpdate True enables the auto update feature, false disables auto update.
	 */
	void setAutoUpdate( bool autoUpdate );

	/**
	 * @brief Manually updates the display.
	 */
	void update();

	/**
	 * @brief Draws a single pixel onto the screen.
	 *
	 * @param point Coordinates where to draw the pixel.
	 * @param color The color to give to the pixel.
	 */
	void drawPixel( const IMonochromeDisplay::Point & point ,
					IMonochromeDisplay::PixelColor color = IMonochromeDisplay::Black );

	/**
	 * @brief Draws a line onto the display from begin to end.
	 *
	 * @param begin 	Start point of the line.
	 * @param end 		End point of the line.
	 * @param color 	Color of the line stroke.
	 */
	void drawLine( const IMonochromeDisplay::Point & begin ,
			       const IMonochromeDisplay::Point & end ,
			       IMonochromeDisplay::PixelColor color = IMonochromeDisplay::Black );

	/**
	 * @brief Draws a rectangle onto the display.
	 *
	 * @param topLeft 	The point at the top left of the rectangle.
	 * @param size 		The rectangle's size to draw (including first point).
	 * @param stroke 	The stroke color, no stroke is drawn if stroke is Undefined.
	 * @param fill 		The fill color, no fill is drawn if fill is Undefined.
	 */
	void drawRectangle( const IMonochromeDisplay::Point & topLeft ,
						const IMonochromeDisplay::Size & size ,
			            IMonochromeDisplay::PixelColor stroke = IMonochromeDisplay::Black ,
			            IMonochromeDisplay::PixelColor fill = IMonochromeDisplay::Undefined );

	/**
	 * @brief Draws a ellipse onto the display.
	 *
	 * @param topLeft 	The point at the top left of the ellipse.
	 * @param size 		The ellipse's size to draw (including first point).
	 * @param stroke 	The stroke color, no stroke is drawn if stroke is Undefined.
	 * @param fill 		The fill color, no fill is drawn if fill is Undefined.
	 */
#if (DISPLAYPAINTER_USE_EXTENED_FEATURES != 0)
	void drawEllipse( const IMonochromeDisplay::Point & topLeft ,
					  const IMonochromeDisplay::Size & size ,
			          IMonochromeDisplay::PixelColor stroke = IMonochromeDisplay::Black ,
			          IMonochromeDisplay::PixelColor fill = IMonochromeDisplay::Undefined );
#endif

	/**
	 * @brief Draws a text onto a display.
	 *
	 * @param topLeft 	The point at the top left of the text block.
	 * @param text 		The text to render onto the screen.
	 * @param color 	The foreground color of the text.
	 */
	void drawText( const IMonochromeDisplay::Point & topLeft ,
			       const std::string & text ,
			       IMonochromeDisplay::PixelColor color = IMonochromeDisplay::Black );

	/**
	 * @brief Draws the given monochrome bitmap (1 bit per pixel, no aligned) into the display at the given position.
	 *
	 * @param topLeft 	Coordinate of the Top left corner of the image on the screen.
	 * @param size 		Dimensions of the bitmap in pixels.
	 * @param bitmap	Pointer to the first bit of the bitmap. Note the first pixel has to be aligned 8 bit.
	 */
#if (DISPLAYPAINTER_USE_EXTENED_FEATURES != 0)
	void drawBitmap( const IMonochromeDisplay::Point & topLeft ,
					 const IMonochromeDisplay::Size & size ,
					 const uint8_t * const bitmap );
#endif

private:
	// Creates and optinally initializes the off-screen bitmap.
	void _createOffsetBitmap( bool sync = false );

	// Changes the pixel at the given position either inside the display or the off-screen bitmap.
	void _setPixel( const IMonochromeDisplay::Point & p  , IMonochromeDisplay::PixelColor color );

	// Synchronizes the off-screen bitmap into the display memory.
	void _syncDisplay( int startLine , int endLine );

	// The display and his acceleration mask to work on.
	IMonochromeDisplay * _display;
	IMonochromeDisplay::AcceleratedDrawingMethods _accelMethods;

	// Attributes needed for offscreen rendering operation.
	uint8_t * _offscreenBitmap;
	bool _disableSync;
	bool _autoUpdate;

	// Font table to find text character definitions from.
	static const uint8_t FontLookup [][6];
};
