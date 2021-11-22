
#include <cstring>
#include <algorithm>
#include <assert.h>
#include "platform-config.h"
#if (DISPLAYPAINTER_USE_EXTENED_FEATURES != 0)
  #include "fixmath/fix16.h"
  #include "fixmath/fix16.hpp"
#endif // DISPLAYPAINTER_USE_EXTENED_FEATURES
#include "MonochromeDisplayPainter"

MonochromeDisplayPainter::MonochromeDisplayPainter( IMonochromeDisplay * display , bool autoUpdate )
	: _display( display ) , _offscreenBitmap( NULL ) , _disableSync( false ) , _autoUpdate( autoUpdate )
{
	// Ask the display for his accelerated drawing methods.
	if ( display ) _accelMethods = display->acceleratedDrawingMethods();
}

MonochromeDisplayPainter::~MonochromeDisplayPainter()
{
	// End painting.
	end();
}

void MonochromeDisplayPainter::begin( IMonochromeDisplay * display )
{
	// End the work relationship with the old display if there was any.
	end();

	// Set display pointer and ask the device for his capabilities.
	_display = display;
	if ( display ) _accelMethods = display->acceleratedDrawingMethods();
}

void MonochromeDisplayPainter::end()
{
	// Commit changes if the display exists.
	if ( _display && !_autoUpdate )
		update();

	// We can not work anymore with the recent display object.
	_display = NULL;

	// If we used an off-screen bitmap, we have to remove it.
	if ( _offscreenBitmap )
	{
		delete[] _offscreenBitmap;
		_offscreenBitmap = NULL;
	}
}

void MonochromeDisplayPainter::clear( IMonochromeDisplay::PixelColor color )
{
	assert( _display );
	if ( _display )
	{
		// Try first the accelerated clear method.
		if ( _accelMethods.clearAccelerated )
		{
			_display->clear( color );

			if ( _offscreenBitmap )
			{
				memset( _offscreenBitmap , color == IMonochromeDisplay::White ? 0x00 : 0xFF ,
						( ( _display->resolution().width * _display->resolution().height ) + 7 ) / 8 );
			}
		}

		// Try buffer copy method.
		else if ( _accelMethods.memoryCopyAccelerated )
		{
			// Create the off-screen buffer if it exists not already without sync from display.
			_createOffsetBitmap();

			// Initialize the bitmask.
			memset( _offscreenBitmap , color == IMonochromeDisplay::White ? 0x00 : 0xFF ,
					( ( _display->resolution().width * _display->resolution().height ) + 7 ) / 8 );

			// Sync the off-screen buffer to the real display.
			if ( _autoUpdate )
				_syncDisplay( 0 , _display->resolution().height );
		}

		// Fall back to software rendered method at the worst case.
		else
		{
			drawRectangle( IMonochromeDisplay::Point( 0 , 0 ) , _display->resolution() ,
					       IMonochromeDisplay::Undefined , color );
		}
	}
}

bool MonochromeDisplayPainter::autoUpdate() const
{
	return _autoUpdate;
}
void MonochromeDisplayPainter::setAutoUpdate( bool autoUpdate )
{
	_autoUpdate = autoUpdate;
}

void MonochromeDisplayPainter::update()
{
	_syncDisplay( 0 , _display->resolution().height );
}

void MonochromeDisplayPainter::drawPixel( const IMonochromeDisplay::Point & point ,
										  IMonochromeDisplay::PixelColor color )
{
	assert( _display );
	if ( _display )
	{
		// Try first buffer copy method if auto update is disabled.
		if ( !_autoUpdate && _accelMethods.memoryCopyAccelerated )
		{
			_createOffsetBitmap( true );
			_setPixel( point , color );
		}

		// Set the pixel directly on the display screen.
		else
		{
			_display->setPixel( point , color );

			// Draw it to the off-screen buffer too if needed to be in sync with the two.
			if ( _offscreenBitmap ) _setPixel( point , color );
		}
	}
}

void MonochromeDisplayPainter::drawLine( const IMonochromeDisplay::Point & begin ,
										 const IMonochromeDisplay::Point & end ,
										 IMonochromeDisplay::PixelColor color )
{
	assert( _display );
	if ( _display )
	{
		// Try first the accelerated method.
		if ( _accelMethods.drawLineAccelerated && !_offscreenBitmap )
		{
			_display->drawLine( begin , end , color );
		}

		// Fall back to either memory copy or direct pixel rendering method.
		else
		{
			// If memory copy acceleration is supported, ensure that the local buffer is ready.
			if ( _accelMethods.memoryCopyAccelerated )
				_createOffsetBitmap( true );

			// Values used for line calculations.
			int dx , dy , ix , iy  , err , i;
			IMonochromeDisplay::Point p = begin;

			// Calculate deltas.
			dx = end.x - begin.x;
			dy = end.y - begin.y;

			// Detect direction.
			if ( dx >= 0 ) ix = 1;
			else
			{
				ix = -1;
				dx = abs( dx );
			}

			if ( dy >= 0 ) iy = 1;
			else
			{
				iy = -1;
				dy = abs( dy );
			}

			// Draw line for the case that the X axis delta dominates.
			if ( dx > dy )
			{
				err = 2 * dy - dx;

				for ( i = 0 ; i <= dx ; i++ )
				{
					_setPixel( p , color );
					if ( err >= 0 )
					{
						err -= 2 * dx;
						p.y += iy;
					}
					err += 2 * dy;
					p.x += ix;
				}
			}

			// Draw line for the case that the Y axis delta dominates.
			else
			{
				err = 2 * dx - dy;

				for ( i = 0 ; i <= dy ; i++ )
				{
					_setPixel( p , color );
					if (err >= 0)
					{
						err -= 2 * dy;
						p.x += ix;
					}
					err += 2 * dx;
					p.y += iy;
				}
			}

			// Sync affected lines with hardware screen.
			if ( _autoUpdate )
				_syncDisplay( std::min( begin.y , end.y ) , std::max( begin.y , end.y ) + 1 );
		}
	}
}

void MonochromeDisplayPainter::drawRectangle( const IMonochromeDisplay::Point & topLeft ,
											  const IMonochromeDisplay::Size & size ,
											  IMonochromeDisplay::PixelColor stroke ,
											  IMonochromeDisplay::PixelColor fill )
{
	assert( _display );
	if ( _display )
	{
		// Do we need to fill the rectangle?
		if ( fill != IMonochromeDisplay::Undefined )
		{
			// Try first the accelerated method.
			if ( _accelMethods.drawRectangeFillAccelerated && !_offscreenBitmap )
			{
				_display->fillRectangle( topLeft , size , fill );
			}

			// Fall back to software rendered method by either buffer copy or direct pixel access.
			else
			{
				// If memory copy acceleration is supported, ensure that the local buffer is ready.
				if ( _accelMethods.memoryCopyAccelerated )
					_createOffsetBitmap( true );

				// Calculate fill surface.
				IMonochromeDisplay::Point start = topLeft;
				IMonochromeDisplay::Point end( topLeft.x + size.width - 1 , topLeft.y + size.height - 1 );

				// If there is a stroke, we can fill less points.
				if ( stroke != IMonochromeDisplay::Undefined )
				{
					start.x++;
					start.y++;
					end.x--;
					end.y--;
				}

				// Fill the rectangle.
				for ( int x =start.x ; x <= end.x ; ++x )
					for ( int y = start.y ; y <= end.y ; ++y )
						_setPixel( IMonochromeDisplay::Point( x , y ) , fill );
			}
		}

		// Draw border if needed.
		if ( stroke != IMonochromeDisplay::Undefined )
		{
			// Try first the accelerated method.
			if ( _accelMethods.drawRectangleStrokeAccelerated && !_offscreenBitmap )
			{
				_display->drawRectangle( topLeft , size , stroke );
			}

			// Fall back to software rendered method.
			else
			{
				// If memory copy acceleration is supported, ensure that the local buffer is ready.
				if ( _accelMethods.memoryCopyAccelerated )
				{
					_createOffsetBitmap( true );

					// Do not flush the display buffer 4 times to the display!
					_disableSync = true;
				}

				drawLine( topLeft ,
						  IMonochromeDisplay::Point( topLeft.x , topLeft.y + size.height - 1 ) ,
						  stroke );
				drawLine( IMonochromeDisplay::Point( topLeft.x , topLeft.y + size.height - 1 ) ,
						  IMonochromeDisplay::Point( topLeft.x + size.width - 1 , topLeft.y + size.height - 1 ) ,
						  stroke);
				drawLine( IMonochromeDisplay::Point( topLeft.x + size.width - 1 , topLeft.y + size.height - 1 ) ,
						  IMonochromeDisplay::Point( topLeft.x + size.width - 1 , topLeft.y ) ,
						  stroke );
				drawLine( IMonochromeDisplay::Point( topLeft.x + size.width - 1 , topLeft.y ) ,
						  topLeft ,
						  stroke );
			}
		}

		// Finally sync screen.
		_disableSync = false;
		if ( _autoUpdate )
			_syncDisplay( topLeft.y , topLeft.y + size.height );
	}
}

#if (DISPLAYPAINTER_USE_EXTENED_FEATURES != 0)
void MonochromeDisplayPainter::drawEllipse( const IMonochromeDisplay::Point & topLeft ,
				  	  	  	  	  	  	  	const IMonochromeDisplay::Size & size ,
				  	  	  	  	  	  	  	IMonochromeDisplay::PixelColor stroke ,
				  	  	  	  	  	  	  	IMonochromeDisplay::PixelColor fill )
{
	assert( _display );
	if ( _display )
	{
		// Do we need to fill the ellipse?
		if ( fill != IMonochromeDisplay::Undefined )
		{
			// Try first the accelerated method.
			if ( _accelMethods.drawEllipseFillAccelerated && !_offscreenBitmap )
			{
				_display->fillEllipse( topLeft , size , fill );
			}

			// Fall back to software rendered method either per direct pixel access or buffer copy.
			else
			{
				// If memory copy acceleration is supported, ensure that the local buffer is ready.
				if ( _accelMethods.memoryCopyAccelerated )
					_createOffsetBitmap( true );

				// Fill ellipse.
				Fix16 radiusX = Fix16( size.width - 1 ) / 2;
				Fix16 radiusY = Fix16( size.height - 1 ) / 2;
				Fix16 centerX = radiusX + topLeft.x;
				Fix16 centerY = radiusY + topLeft.y;


				for ( Fix16 x = topLeft.x ; x < topLeft.x + size.width ; x += 1 )
					for ( Fix16 y = topLeft.y ; y < topLeft.y + size.height ; y += 1 )
					{
						if ( ( ( x - centerX ) * ( x - centerX ) ) / ( radiusX * radiusX ) +
							 ( ( y - centerY ) * ( y - centerY ) ) / ( radiusY * radiusY ) < 1 )
						{
							_setPixel( IMonochromeDisplay::Point( x , y ) , fill );
						}
					}
			}
		}

		// Draw border if needed.
		if ( stroke != IMonochromeDisplay::Undefined )
		{
			// Try first the accelerated method.
			if ( _accelMethods.drawEllipseStrokeAccelerated && !_offscreenBitmap )
			{
				_display->drawEllipse( topLeft , size , stroke );
			}

			// Fall back to software rendered method either per direct pixel access or buffer copy.
			else
			{
				if ( _accelMethods.memoryCopyAccelerated )
					_createOffsetBitmap( true );

				// Draw the border of the ellipse.
				Fix16 radiusX = Fix16( size.width - 1 ) / Fix16( 2 );
				Fix16 radiusY = Fix16( size.height - 1 ) / Fix16( 2 );
				Fix16 centerX = Fix16( topLeft.x ) + radiusX;
				Fix16 centerY = Fix16( topLeft.y ) + radiusY;
				Fix16 x = 0;
				Fix16 y = radiusY;
				Fix16 angle = 0;
				Fix16 range = radiusX;

				do
				{
					_setPixel( IMonochromeDisplay::Point( Fix16( centerX + x + 0.0 ) ,
														  Fix16( centerY + y + 0.0 ) ) , stroke );
					_setPixel( IMonochromeDisplay::Point( Fix16( centerX + x + 0.0 ) ,
														  Fix16( centerY - y + 0.0 ) ) , stroke );
					_setPixel( IMonochromeDisplay::Point( Fix16( centerX - x + 0.0 ) ,
														  Fix16( centerY - y + 0.0 ) ) , stroke );
					_setPixel( IMonochromeDisplay::Point( Fix16( centerX - x + 0.0 ) ,
														  Fix16( centerY + y + 0.0 ) ) , stroke );

					angle += 0.05;
					x = ( radiusX * angle.cos() );
					y = ( radiusY * angle.sin() );

				} while( angle <= range );
			}
		}

		// Finally sync screen.
		if ( _autoUpdate )
			_syncDisplay( topLeft.y , topLeft.y + size.height );
	}
}
#endif // DISPLAYPAINTER_USE_EXTENED_FEATURES

void MonochromeDisplayPainter::drawText( const IMonochromeDisplay::Point & topLeft ,
			   	   	   	   	   	   	     const std::string & text ,
			   	   	   	   	   	   	     IMonochromeDisplay::PixelColor color )
{
	assert( _display );
	if ( _display )
	{
		// Well we need some text.
		if ( !text.empty() )
		{
			// Try first the accelerated method.
			if ( _accelMethods.drawTextAccelerated && !_offscreenBitmap )
			{
				_display->drawText( topLeft , text , color );
			}
			else
			{
				// If memory copy acceleration is supported, ensure that the local buffer is ready.
				if ( _accelMethods.memoryCopyAccelerated )
					_createOffsetBitmap( true );

				IMonochromeDisplay::Point p = topLeft;
				unsigned int textPosition = 0;

				// Draw the text.
				while( p.x <= ( _display->resolution().width - 5 ) && textPosition < text.size() )
				{
					unsigned char character = text[textPosition];
					if ( character > 223 ) character -= 86;
					if ( character < 32 || character > 177 ) character = ' ';

					for ( int x = 0 ; x < 5 ; ++x )
						for ( int y = 0 ; y < 7 ; ++y )
							if ( FontLookup[character - 32][x] & ( 1 << y ) )
								_setPixel( IMonochromeDisplay::Point( p.x + x , p.y + y ) , color );

					textPosition++;
					p.x += 6;
				}

				// Finally flush the data to the display.
				if ( _autoUpdate )
					_syncDisplay( topLeft.y , topLeft.y + 7 );
			}
		}
	}
}

#if (DISPLAYPAINTER_USE_EXTENED_FEATURES != 0)
void MonochromeDisplayPainter::drawBitmap( const IMonochromeDisplay::Point & topLeft ,
				 	 	 	 	 	 	   const IMonochromeDisplay::Size & size ,
				 	 	 	 	 	 	   const uint8_t * const bitmap )
{
	assert( _display );
	if ( _display )
	{
		// If memory copy acceleration is supported, ensure that the local buffer is ready.
		if ( _accelMethods.memoryCopyAccelerated )
			_createOffsetBitmap( true );

		for ( int x = 0 ; x < size.width ; ++x )
			for ( int y = 0 ; y < size.height ; ++y )
			{
				unsigned int bitIndex = x + y * size.width;
				_setPixel( IMonochromeDisplay::Point( x , y ) ,
						   *( bitmap + ( bitIndex / 8 ) ) & ( 1 << ( bitIndex % 8 ) ) ?
						   IMonochromeDisplay::Black :
						   IMonochromeDisplay::White );
			}

		// Finally flush the data to the display.
		if ( _autoUpdate )
			_syncDisplay( topLeft.y , topLeft.y + size.height );
	}
}
#endif // DISPLAYPAINTER_USE_EXTENED_FEATURES

void MonochromeDisplayPainter::_createOffsetBitmap( bool copyFromDisplay )
{
	// Do we need to create the bitmap or does it already exists?
	if ( !_offscreenBitmap )
	{
		// Allocate memory for off-screen bitmap.
		_offscreenBitmap =
			new uint8_t[( ( _display->resolution().width * _display->resolution().height ) + 7 ) / 8];

		// If needed, we copy the actual content of the display to the off-screen buffer.
		if ( copyFromDisplay )
			_display->readDisplayMemory( 0 , _offscreenBitmap ,
					_display->resolution().width * _display->resolution().height );
	}
}

void MonochromeDisplayPainter::_setPixel( const IMonochromeDisplay::Point & p , IMonochromeDisplay::PixelColor color )
{
	// Check pixel bounds.
	if ( p.x >= 0 && p.x < _display->resolution().width && p.y >= 0 && p.y < _display->resolution().height )
	{
		// Do we need to draw into the off-screen bitmap?
		if ( _offscreenBitmap )
		{
			// Draw into the off-screen bitmap.
			if ( color == IMonochromeDisplay::White )
				*( _offscreenBitmap + ( p.y * _display->resolution().width + p.x ) / 8 ) &=
					~( 1 << ( ( p.y * _display->resolution().width + p.x ) % 8 ) );

			if ( color == IMonochromeDisplay::Black )
				*( _offscreenBitmap + ( p.y * _display->resolution().width + p.x ) / 8 ) |=
					( 1 << ( ( p.y * _display->resolution().width + p.x ) % 8 ) );
		}
		else
		{
			// Draw directly onto the display.
			_display->setPixel( p , color );
		}
	}
}

void MonochromeDisplayPainter::_syncDisplay( int startLine , int endLine )
{
	// Do we need to sync the display.
	if ( _offscreenBitmap && !_disableSync )
	{
		// Check line (Y) bounds.
		if ( startLine >= 0 && startLine <= _display->resolution().height &&
			 endLine >= 0 && endLine <= _display->resolution().height )
		{
			size_t bitOffset = std::min( startLine , endLine ) * _display->resolution().width;
			size_t numberOfBits = _display->resolution().width * abs( endLine - startLine );
			size_t modulo = bitOffset % 8;

			// Align the start address and the number of bits to 8 bit bounds.
			if ( modulo )
			{
				bitOffset -= modulo;
				numberOfBits += modulo;
			}

			// Write the off-screen bitmap buffer to the display.
			_display->writeDisplayMemory( bitOffset / 8 , _offscreenBitmap + bitOffset / 8 , numberOfBits );
		}
	}
}
