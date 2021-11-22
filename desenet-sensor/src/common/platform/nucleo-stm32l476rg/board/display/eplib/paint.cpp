#include <cstdlib>
#include <cstring>  // memset()
#include <cmath>
#include "paint.h"

namespace ep {

Paint::Paint()
{
    buffer = nullptr;
}
void Paint::initialize(ep::Display * epd)
{
    this->epd = epd;
}

/******************************************************************************
function:	Set up buffer
parameter:
    buffer  :   Pointer to the buffer cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint::setBuffer(uint8_t * buffer, uint16_t Width, uint16_t Height, uint16_t Rotate, uint16_t Color)
{
    this->buffer = buffer;

    this->width = Width;
    this->height = Height;

    this->widthMemory = Width;
    this->heightMemory = Height;
    this->color = Color;
    this->widthByte = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
    this->heightByte = Height;

    this->rotate = Rotate;

    this->mirror = MIRROR_NONE;

    if (Rotate == EP_DISPLAY_ROTATE_0 || Rotate == EP_DISPLAY_ROTATE_180)
    {
        this->width = Width;
        this->height = Height;
    }
    else
    {
        this->width = Height;
        this->height = Width;
    }

    this->pixelCount = width * height;
    this->bytesCount = pixelCount / 8;

    this->buffer = buffer;
}

/******************************************************************************
function:	Select buffer rotate
parameter:
    Rotate   :   0,90,180,270
******************************************************************************/
void Paint::setRotate(Rotation Rotate)
{
    if (Rotate == EP_DISPLAY_ROTATE_0 || Rotate == EP_DISPLAY_ROTATE_90 || Rotate == EP_DISPLAY_ROTATE_180 || Rotate == EP_DISPLAY_ROTATE_270)
    {
        this->rotate = Rotate;
    }
}

/******************************************************************************
function:	Select buffer mirror
parameter:
    mirror   :       Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void Paint::setMirroring(uint8_t mirror)
{
    if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
        mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN)
    {
        this->mirror = mirror;
    }
}

/******************************************************************************
function:	Draw Pixels
parameter:
    Xpoint  :   At point X
    Ypoint  :   At point Y
    Color   :   Painted colors
******************************************************************************/
void Paint::setPixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color, bool refresh)
{
    uint16_t X, Y;
    uint32_t Addr;
    uint8_t Rdata;
    if (Xpoint > this->width || Ypoint > this->height)
    {
        return;
    }

    switch (this->rotate)
    {
    case 0:
        X = Xpoint;
        Y = Ypoint;
        break;
    case 90:
        X = this->widthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = this->widthMemory - Xpoint - 1;
        Y = this->heightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = this->heightMemory - Xpoint - 1;
        break;
    default:
        assert(false);
        return;
    }

    switch (this->mirror)
    {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        X = this->widthMemory - X - 1;
        break;
    case MIRROR_VERTICAL:
        Y = this->heightMemory - Y - 1;
        break;
    case MIRROR_ORIGIN:
        X = this->widthMemory - X - 1;
        Y = this->heightMemory - Y - 1;
        break;
    default:
        return;
    }

    if (X > this->widthMemory || Y > this->heightMemory)
    {
        return;
    }

    Addr = X / 8 + Y * this->widthByte;
    Rdata = this->buffer[Addr];
    if (Color == EP_DISPLAY_BLACK)
        this->buffer[Addr] = Rdata & ~(0x80 >> (X % 8));
    else
        this->buffer[Addr] = Rdata | (0x80 >> (X % 8));

    if (refresh)
    {
        update();
    }
}

/******************************************************************************
function:	Clear the color of the picture
parameter:
    Color   :   Painted colors
******************************************************************************/
void Paint::clear(uint8_t Color, bool refresh)
{
    memset(this->buffer, Color, bytesCount);

    if (refresh)
    {
        update();
    }
}

/******************************************************************************
function:	Clear the color of a window
parameter:
    Xstart :   x starting point
    Ystart :   Y starting point
    Xend   :   x end point
    Yend   :   y end point
******************************************************************************/
void Paint::clearWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color)
{
    uint16_t X, Y;
    for (Y = Ystart; Y < Yend; Y++)
    {
        for (X = Xstart; X < Xend; X++)
        { //8 pixel =  1 byte
            setPixel(X, Y, Color);
        }
    }
    update();
}

void Paint::update()
{
    epd->display(buffer);
}

/******************************************************************************
function:	Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		:   The Xpoint coordinate of the point
    Ypoint		:   The Ypoint coordinate of the point
    Color		:   Set color
    Dot_Pixel	:	point size
    refresh		:	Send or not refresh command
******************************************************************************/
void Paint::drawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color,
                      DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE, bool refresh)
{
    int16_t XDir_Num, YDir_Num;
    if (Xpoint > this->width || Ypoint > this->height)
    {
        return;
    }

    if (DOT_STYLE == DOT_FILL_AROUND)
    {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++)
        {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++)
            {
                if (Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                setPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color, refresh);
            }
        }
    }
    else
    {
        for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++)
        {
            for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++)
            {
                setPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color, refresh);
            }
        }
    }

    if (refresh)
    {
        update();
    }
}

/******************************************************************************
function:	Draw a line of arbitrary slope
parameter:
    Xstart 		：	Starting Xpoint point coordinates
    Ystart 		：	Starting Xpoint point coordinates
    Xend   		：	End point Xpoint coordinate
    Yend   		：	End point Ypoint coordinate
    Color  		：	The color of the line segment
	refresh		:	Send or not refresh command
******************************************************************************/
void Paint::drawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
                     uint16_t Color, LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel, bool refresh)
{
    uint16_t Xpoint, Ypoint;
    int dx, dy;
    int XAddway, YAddway;
    int Esp;
    char Dotted_Len;
    if (Xstart > this->width || Ystart > this->height ||
        Xend > this->width || Yend > this->height)
    {
        return;
    }

    Xpoint = Xstart;
    Ypoint = Ystart;
    dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    XAddway = Xstart < Xend ? 1 : -1;
    YAddway = Ystart < Yend ? 1 : -1;

    //Cumulative error
    Esp = dx + dy;
    Dotted_Len = 0;

    for (;;)
    {
        Dotted_Len++;
        //Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0)
        {
            drawPoint(Xpoint, Ypoint, EP_DISPLAY_IMAGE_BACKGROUND, Dot_Pixel, DOT_STYLE_DFT, refresh);
            Dotted_Len = 0;
        }
        else
        {
            drawPoint(Xpoint, Ypoint, Color, Dot_Pixel, DOT_STYLE_DFT, refresh);
        }
        if (2 * Esp >= dy)
        {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx)
        {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }

    if (refresh)
    {
        update();
    }
}

/******************************************************************************
function:	Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Filled : Whether it is filled--- 1 solid 0：empty
    refresh : Send or not refresh command
******************************************************************************/
void Paint::drawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
                          uint16_t Color, DRAW_FILL Filled, DOT_PIXEL Dot_Pixel,bool refresh)
{
    uint16_t Ypoint;
    if (Xstart > this->width || Ystart > this->height ||
        Xend > this->width || Yend > this->height)
    {
        return;
    }

    if (Filled)
    {
        for (Ypoint = Ystart; Ypoint < Yend; Ypoint++)
        {
            drawLine(Xstart, Ypoint, Xend, Ypoint, Color, LINE_STYLE_SOLID, Dot_Pixel, false);
        }
    }
    else
    {
        drawLine(Xstart, Ystart, Xend, Ystart, Color, LINE_STYLE_SOLID, Dot_Pixel, false);
        drawLine(Xstart, Ystart, Xstart, Yend, Color, LINE_STYLE_SOLID, Dot_Pixel, false);
        drawLine(Xend, Yend, Xend, Ystart, Color, LINE_STYLE_SOLID, Dot_Pixel, false);
        drawLine(Xend, Yend, Xstart, Yend, Color, LINE_STYLE_SOLID, Dot_Pixel, false);
    }

    if (refresh)
    {
    	update();
    }
}

/******************************************************************************
function:	Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Filled    : Whether it is filled: 1 filling 0：Do not
	refresh	  :	Send or not refresh command
******************************************************************************/
void Paint::drawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius,
                       uint16_t Color, DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel, bool refresh)
{
    int16_t Esp, sCountY;
    int16_t XCurrent, YCurrent;
    if (X_Center > this->width || Y_Center >= this->height)
    {
        return;
    }

    //Draw a circle from(0, R) as a starting point
    XCurrent = 0;
    YCurrent = Radius;

    //Cumulative error,judge the next point of the logo
    Esp = 3 - (Radius << 1);
    if (Draw_Fill == DRAW_FILL_FULL)
    {
        while (XCurrent <= YCurrent)
        { //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++)
            {
                drawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false); //1
                drawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false); //2
                drawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false); //3
                drawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false); //4
                drawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false); //5
                drawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false); //6
                drawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false); //7
                drawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT, false);
            }
            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else
            {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
    else
    { //Draw a hollow circle
        while (XCurrent <= YCurrent)
        {
            drawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //1
            drawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //2
            drawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //3
            drawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //4
            drawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //5
            drawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //6
            drawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //7
            drawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT, false); //0

            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else
            {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }

    if (refresh)
    {
    	update();
    }
}

/******************************************************************************
function:	Show characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the characters
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the character
    Color_Foreground : Select the foreground color of the character
    refresh			 :	Send or not refresh command
******************************************************************************/
void Paint::drawChar(uint16_t Xpoint, uint16_t Ypoint, const char Acsii_Char,
                     sFONT *Font, uint16_t Color_Background, uint16_t Color_Foreground, bool refresh)
{
    uint16_t Page, Column;
    uint32_t Char_Offset;
    const unsigned char *ptr;

    if (Xpoint > this->width || Ypoint > this->height)
    {
        return;
    }

    Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    ptr = &Font->table[Char_Offset];

    for (Page = 0; Page < Font->Height; Page++)
    {
        for (Column = 0; Column < Font->Width; Column++)
        {
            //To determine whether the font background color and screen background color is consistent
            if (EP_DISPLAY_FONT_BACKGROUND == Color_Background)
            { //this process is to speed up the scan
                if (*ptr & (0x80 >> (Column % 8)))
                    setPixel(Xpoint + Column, Ypoint + Page, Color_Foreground, refresh);
            }
            else
            {
                if (*ptr & (0x80 >> (Column % 8)))
                {
                    setPixel(Xpoint + Column, Ypoint + Page, Color_Foreground, refresh);
                }
                else
                {
                    setPixel(Xpoint + Column, Ypoint + Page, Color_Background, refresh);
                }
            }
            //One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        } // Write a line
        if (Font->Width % 8 != 0)
            ptr++;
    } // Write all

    if (refresh)
    {
        update();
    }
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the character
    Color_Foreground : Select the foreground color of the character
    refresh			 :	Send or not refresh command
******************************************************************************/
void Paint::drawString(uint16_t Xstart, uint16_t Ystart, const char *pString,
                       sFONT *Font, uint16_t Color_Background, uint16_t Color_Foreground, bool refresh)
{
    uint16_t Xpoint = Xstart;
    uint16_t Ypoint = Ystart;

    if (Xstart > this->width || Ystart > this->height)
    {
        return;
    }

    while (*pString != '\0')
    {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + Font->Width) > this->width)
        {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint + Font->Height) > this->height)
        {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        drawChar(Xpoint, Ypoint, *pString, Font, Color_Background, Color_Foreground, false);

        //The next character of the address
        pString++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }

    if (refresh)
    {
    	update();
    }
}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
******************************************************************************/
void Paint::drawBitmap(const uint8_t * buffer)
{
    memcpy(this->buffer, buffer, bytesCount);

    update();
}

} // namespace ep
