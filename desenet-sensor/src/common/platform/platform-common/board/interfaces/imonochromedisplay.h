#pragma once
#include "platform-config.h"
#include <assert.h>
#include <stdint.h>
#include <cstddef>
#include <string>

/**
 * @ingroup interfaces
 * @brief Abstracts a simple monochrome display, with either unaccelerated or accelerated drawing methods.
 *
 * Note that if you need a common way to paint into displays with different accelerated features, use the
 * MonochromeDisplayPainter class, the class uses for each drawing instruction the  most effective method to
 * draw into the display.
 */
class IMonochromeDisplay
{
public:
	/**
	 * @brief The definitions for the pixel colors. Quite a short list for monochrome displays...
	 */
	enum PixelColor
	{
		Undefined 	=  0 ,	///< Undefined color, use if you want no stroke or no fill.
		White 		= -1 ,	///< White pixel value.
		Black 		=  1 	///< Black pixel value.
	};

	/**
	 * @brief A bit-mask defining the possible accelerated drawing operations a display can offer.
	 *
	 * The MonochromeDisplayPainter class will use accelerated methods if available and will fall back
	 * to software based rendering methods if the display does not support the accelerated operations.
	 */
	struct AcceleratedDrawingMethods
	{
		unsigned clearAccelerated : 1;					///< @brief The display supports to clear screen.
		unsigned memoryCopyAccelerated : 1;				///< @brief The display supports burst access to the display memory.
		unsigned drawLineAccelerated : 1;				///< @brief The display supports drawing simple lines.
		unsigned drawRectangleStrokeAccelerated : 1;	///< @brief The display supports drawing strokes of rectangles.
		unsigned drawRectangeFillAccelerated : 1;		///< @brief The display supports filling rectangles.
		unsigned drawEllipseStrokeAccelerated : 1;		///< @brief The display supports drawing ellipse strokes.
		unsigned drawEllipseFillAccelerated : 1;		///< @brief The display supports filling ellipses.
		unsigned drawTextAccelerated : 1;				///< @brief The display supports text drawing.
	};

	/**
	 * @brief This class represents a point in a two-dimensional space and is used to draw into the display.
	 */
	class Point
	{
	public:
		/**
		 * @brief Constructor, initializes the x and y coordinates of the point.
		 *
		 * @param x The X coordinate.
		 * @param y The Y coordinate.
		 */
		inline explicit Point( int x = 0 , int y = 0 ) : x( x ) , y( y ) {}
		inline Point(std::initializer_list<uint8_t> values)
		{
			assert(values.size() == 2);
			std::initializer_list<uint8_t>::const_iterator it = values.begin();

			x = *it++;
			y = *it;
		}

		int x;		///< @brief The X coordinate.
		int y;		///< @brief The Y coordinate.
	};

	/**
	 * @brief This class represents a size in a two-dimensional space used to describe the size for all display methods.
	 */
	class Size
	{
	public:
		/**
		 * @brief Constructor, initializes the width and height of a size.
		 *
		 * @param width 	The width.
		 * @param height 	The height.
		 */
		inline explicit Size( int width = 0 , int height = 0 ) : width( width ) , height( height ) {}

		int width;		///< @brief The width.
		int height;		///< @brief The height.
	};

protected:
	// It is just an interface, so constructor and destructor should be not public.
	explicit IMonochromeDisplay() {}
	virtual ~IMonochromeDisplay() {}

public:
	/**
	 * @brief Returns the resolution of the display in pixels.
	 *
	 * This method is REQUIRED.
	 *
	 * @return Resolution in pixels.
	 */
	virtual Size resolution() const = 0;

	/**
	 * @brief Returns a bit-mask of the operations that are accelerated.
	 *
	 * This method is REQUIRED.
	 *
	 * @return Bitmask of supported hardware accelerated drawing methods.
	 */
	virtual AcceleratedDrawingMethods acceleratedDrawingMethods() const = 0;

	/**
	 * @brief Initializes all subsystems and the display itself needed for proper operation.
	 *
	 * This method must be called before any other method can be called.
	 *
	 * This method is REQUIRED.
	 *
	 * @return True if the LCD display could be initialized and is ready to be used, false otherwise.
	 */
	virtual bool initialize() = 0;

	/**
	 * @brief Enables the display operation if the display was deactivated before with the disable() method.
	 *
	 * This method is REQUIRED.
	 *
	 * @return True if the display could be enabled (or already was enabled), otherwise false.
	 */
	virtual bool enable() = 0;

	/**
	 * @brief Turns the display off to save energy.
	 *
	 * Returns true if the display could be turned off or false if either the device could not been turned off
	 * by an error or the display does not support this operation at all.
	 *
	 * This method is REQUIRED.
	 *
	 * @return True on success, false on error or if disabling is not implemented.
	 */
	virtual bool disable() = 0;

	/**
	 * @brief Returns the actual value of the pixel addressed by the point p.
	 *
	 * This method is REQUIRED.
	 *
	 * @param point 	Point for which the pixel value should be returned.
	 * @return 			The pixel color or Undefined if the point p is outside the display.
	 */
	virtual PixelColor pixel( const Point & point ) = 0;

	/**
	 * @brief Changes the pixel color at the given position to the given pixel color.
	 *
	 * This method is REQUIRED.
	 *
	 * @param point 	Point for which the pixel value should be changed.
	 * @param color 	The pixel color to set for that point.
	 */
	virtual void setPixel( const Point & point , PixelColor color ) = 0;

	/**
	 * @brief Clears the display if this accelerated method is supported.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param background Pixel color to use for background color.
	 */
	virtual void clear( PixelColor background = White ) { (void)(background); }

	/**
	 * @brief Copies the buffer pointed by sourceBitmap into the screen display if the display supports
	 * 		  this acceleration method.
	 *
	 * Note that the buffer pointed by sourceBitmap must be at least have the size to hold all bits aligned
	 * by 1 byte.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param startAddress The start address inside the display memory.
	 * @param sourceBitmap Pointer to the data to be copied into the display buffer.
	 * @param numberOfBits The total number of bits to copy to the display.
	 */
	virtual void writeDisplayMemory( size_t startAddress , const uint8_t * const sourceBitmap ,
									 size_t numberOfBits )
	{ (void)(startAddress); (void)(sourceBitmap); (void)(numberOfBits); }

	/**
	 * @brief Reads the buffer pointed by sourceBitmap into the screen display if the display supports
	 * 		  this acceleration method.
	 *
	 * Note that the buffer pointed by sourceBitmap must be at least have the size to hold all bits aligned
	 * by 1 byte.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param startAddress 		The start address inside the display memory.
	 * @param destinationBitmap Pointer to the data to be copied into the display buffer.
	 * @param numberOfBits 		The total number of bits to copy to the display.
	 */
	virtual void readDisplayMemory( size_t startAddress , uint8_t * const destinationBitmap , size_t numberOfBits )
	{ (void)(startAddress); (void)(destinationBitmap); (void)(numberOfBits);}

	/**
	 * @brief Draws a line on the display from begin to end with the given pixel color if this accelerated method
	 * 		  is supported by the display.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param begin Start point of the line.
	 * @param end End point of the line.
	 * @param color The color the line should have.
	 */
	virtual void drawLine( const Point & begin , const Point & end , PixelColor color )
	{ (void)(begin); (void)(end); (void)(color); }

	/**
	 * @brief Draws the stroke of a rectangle on the display from begin to end with the given pixel color if
	 * 		  this accelerated method is supported by the display.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param topLeft 	Start point of the rectangle.
	 * @param size 		The dimensions of the rectangle.
	 * @param color 	The color the rectangle stroke should have.
	 */
	virtual void drawRectangle( const Point & topLeft , const Size & size , PixelColor color )
	{ (void)(topLeft); (void)(size); (void)(color); }

	/**
	 * @brief Draws the fill of a rectangle on the display from begin to end with the given pixel color if
	 * 		  this accelerated method is supported by the display.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param topLeft 	Start point of the rectangle.
	 * @param size 		The dimensions of the rectangle.
	 * @param color 	The color the rectangle fill should have.
	 */
	virtual void fillRectangle( const Point & topLeft , const Size & size , PixelColor color )
	{ (void)(topLeft); (void)(size); (void)(color); }

	/**
	 * @brief Draws the stroke of an ellipse on the display from begin to end with the given pixel color if
	 * 		  this accelerated method is supported by the display.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param topLeft 	Start point of the ellipse.
	 * @param size 		The dimensions of the ellipse.
	 * @param color 	The color the ellipse stroke should have.
	 */
	virtual void drawEllipse( const Point & topLeft , const Size & size , PixelColor color )
	{ (void)(topLeft); (void)(size); (void)(color); }

	/**
	 * @brief Draws the fill of an ellipse on the display from begin to end with the given pixel color if
	 * 		  this accelerated method is supported by the display.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param topLeft 	Start point of the ellipse.
	 * @param size 		The dimensions of the ellipse.
	 * @param color 	The color the ellipse fill should have.
	 */
	virtual void fillEllipse( const Point & topLeft , const Size & size , PixelColor color )
	{ (void)(topLeft); (void)(size); (void)(color); }

	/**
	 * @brief Draws the text on the display at the given point with the given pixel color if this accelerated method is
	 * 		  supported by the display.
	 *
	 * The system depends on the size of the characters that have to be 5x7 pixels for all characters.
	 *
	 * This method is OPTIONAL.
	 *
	 * @param topLeft 	Start point of the text to display.
	 * @param text 		The text to render to the display.
	 * @param color 	The color the ellipse fill should have.
	 */
	virtual void drawText( const Point & topLeft , const std::string & text , PixelColor color )
	{ (void)(topLeft); (void)(text); (void)(color); }
};

/**
 * @ingroup interfaces
 * @brief Convenient type for IMonochromeDisplay::PixelColor.
 */
typedef IMonochromeDisplay::PixelColor 	IMDPixelColor;

/**
 * @ingroup interfaces
 * @brief Convenient type for IMonochromeDisplay::Point.
 */
typedef IMonochromeDisplay::Point 	IMDPoint;

/**
 * @ingroup interfaces
 * @brief Convenient type for IMonochromeDisplay::Point.
 */
typedef IMonochromeDisplay::Size 	IMDSize;
