#ifndef EP_PAINT_H
#define EP_PAINT_H

#include <stdint.h>
#include "display.h"
#include "fonts/fonts.h"

/* NOTICES :
 * For the moment, some of these functions aren't compatible with a partial refresh of the display
 * Some functions otherwise assume that we have an array of 2756bytes, take care that the 'clear()' function will not clear all of a 5512 array (for 4gray scale)
*/

/**
 * Display rotate
**/
#define EP_DISPLAY_ROTATE_0         0
#define EP_DISPLAY_ROTATE_90        90
#define EP_DISPLAY_ROTATE_180       180
#define EP_DISPLAY_ROTATE_270       270

/**
 * Display Flip
**/
enum  MIRROR_IMAGE
{
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
};
#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * image color
**/
#define EP_DISPLAY_WHITE                0xFF
#define EP_DISPLAY_BLACK                0x00
#define EP_DISPLAY_RED                  EP_DISPLAY_BLACK

#define EP_DISPLAY_IMAGE_BACKGROUND     EP_DISPLAY_WHITE
#define EP_DISPLAY_FONT_FOREGROUND      EP_DISPLAY_BLACK
#define EP_DISPLAY_FONT_BACKGROUND      EP_DISPLAY_WHITE

/**
 * The size of the point
**/
enum DOT_PIXEL
{
    DOT_PIXEL_1X1  = 1,		// 1 x 1
    DOT_PIXEL_2X2  , 		// 2 X 2
    DOT_PIXEL_3X3  ,		// 3 X 3
    DOT_PIXEL_4X4  ,		// 4 X 4
    DOT_PIXEL_5X5  , 		// 5 X 5
    DOT_PIXEL_6X6  , 		// 6 X 6
    DOT_PIXEL_7X7  , 		// 7 X 7
    DOT_PIXEL_8X8  , 		// 8 X 8
};
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
**/
enum DOT_STYLE
{
    DOT_FILL_AROUND  = 1,		// dot pixel 1 x 1
    DOT_FILL_RIGHTUP  , 		// dot pixel 2 X 2
} ;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
enum LINE_STYLE
{
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} ;

/**
 * Whether the graphic is filled
**/
enum DRAW_FILL
{
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
};

namespace ep {

typedef uint16_t Rotation;

class Paint
{
public:
    Paint();
    void initialize(ep::Display * epd);
    //init and Clear
    void setBuffer(uint8_t * buffer, uint16_t Width, uint16_t Height, uint16_t Rotate, uint16_t Color);
    void setRotate(Rotation Rotate);
    void setMirroring(uint8_t mirror);
    void setPixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color, bool refresh=true);

    void clear(uint8_t Color, bool refresh=true);
    void clearWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color);
    void update();

    //Drawing
    void drawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay, bool refresh=true);
    void drawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel, bool refresh=true);
    void drawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, DRAW_FILL Filled , DOT_PIXEL Dot_Pixel, bool refresh=true);
    void drawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius, uint16_t Color, DRAW_FILL Draw_Fill , DOT_PIXEL Dot_Pixel, bool refresh=true);

    //Display string
    void drawChar(uint16_t Xstart, uint16_t Ystart, const char Acsii_Char, sFONT* Font, uint16_t Color_Background, uint16_t Color_Foreground, bool refresh=true);
    void drawString(uint16_t Xstart, uint16_t Ystart, const char * pString, sFONT* Font, uint16_t Color_Background, uint16_t Color_Foreground, bool refresh=true);

    //pic
    void drawBitmap(const uint8_t * imageBuffer);
private:
    ep::Display * epd;
    uint8_t * buffer;
    uint16_t width;
    uint16_t height;
    uint16_t widthMemory;
    uint16_t heightMemory;
    uint16_t color;
    Rotation rotate;
    uint16_t mirror;
    uint16_t widthByte;
    uint16_t heightByte;
    uint16_t pixelCount;
    uint16_t bytesCount;
};

} // namespace ep
#endif // EP_PAINT_H
