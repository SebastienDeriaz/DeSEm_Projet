#include <cassert>
#include "display.h"

namespace ep {

Display * Display::pInstance = nullptr;

Display::Display()
{
    spi = nullptr;
    pinDC = nullptr;
    pinBusy = nullptr;
    pinReset = nullptr;
    pInstance = nullptr;
}

/******************************************************************************
function:	initialize pointers and send a first configuration to the display
parameter:
    spi      :   device spi used to communicate with the display
    pinDC    :   pin used to say to the display if it's some Data or a command
    pinBusy  :   pin used by the display to say if busy or not
    pinReset :   pin used to perform a reset to the display
******************************************************************************/
void Display::initialize(SPI_HandleTypeDef * spi, Gpo * pinDC, Gpi * pinBusy, Gpo * pinReset)
{
    if (spi && pinDC && pinBusy && pinReset)
    {
        this->spi = spi;
        this->pinDC = pinDC;
        this->pinBusy = pinBusy;
        this->pinReset = pinReset;
        pInstance = this;
    }
    else
    {
        assert(false);
    }
    init();
}

Display * Display::instance()
{
    return pInstance;
}

/******************************************************************************
function:	communicate with the command to the display
parameter:
    command :   value of the data or the command
******************************************************************************/
void Display::SPI_Write(unsigned char command)
{
    HAL_SPI_Transmit(this->spi, &command, 1, -1);
}

/******************************************************************************
function:	write a command to the display (well, to the chip)
parameter:
    command :   value of the command
******************************************************************************/
void Display::writeCMD(uint8_t command)
{
    this->pinDC->off();
    SPI_Write(command);
}

/******************************************************************************
function:	write some data to the display (well, to the chip)
parameter:
    command : value of the data
******************************************************************************/
void Display::writeDATA(uint8_t command)
{
    this->pinDC->on();
    SPI_Write(command);
}

/******************************************************************************
function:	Perform some init instructions for the display
******************************************************************************/
void Display::init()
{
	//GD : This annotation is used to notify that this isn't the settings that was found in Adafruit example code
	//But settings found for a better "short refresh" (most of them is certainly useless however)
    uint8_t buf[5];
    initReset();                    // Electronic paper IC reset
    lcdChkstatus();

    writeCMD(SSD1608_SW_RESET);     // soft reset

    lcdChkstatus();

    // driver output control
    buf[0] = HEIGHT - 1;
    buf[1] = (HEIGHT - 1) >> 8;
    buf[2] = 0x00;
    writeCMD(SSD1608_DRIVER_CONTROL);
    writeDATA(buf[0]);
    writeDATA(buf[1]);
    writeDATA(buf[2]);

    //Gate Driving voltage Control (GD)
    writeCMD(SSD1608_GATE_VOLTAGE);
    writeDATA(0x00);

    // Set dummy line period
    writeCMD(SSD1608_WRITE_DUMMY);
    //writeDATA(0x1B);
    writeDATA(0x1A); //GD

    // Set gate line width
    writeCMD(SSD1608_WRITE_GATELINE);
    //writeDATA(0x0B);
    writeDATA(0x08);	//GD

    // Data entry sequence
    writeCMD(SSD1608_DATA_MODE);
    writeDATA(0x03);

    // Set ram X start/end postion
    buf[0]=WIDTH / 8 - 1;
    writeCMD(SSD1608_SET_RAMXPOS);
    writeDATA(0x00);
    writeDATA(buf[0]);

    // Set ram Y start/end postion
    buf[0]=HEIGHT - 1;
    buf[1]=(HEIGHT - 1) >> 8;
    writeCMD(SSD1608_SET_RAMYPOS);
    writeDATA(0x00);
    writeDATA(0x00);
    writeDATA(buf[0]);
    writeDATA(buf[1]);

    // Vcom Voltage
    writeCMD(SSD1608_WRITE_VCOM);
    //writeDATA(0x70);
    writeDATA(0xA8);	//GD

    writeCMD(SSD1608_WRITE_LUT);
    for(uint32_t i = 0; i<30 ; i++){
        writeDATA(LUT_DATA[i]);
    }

    // border color
    buf[0] = 0x00;
    writeCMD(SSD1608_WRITE_BORDER);
    writeDATA(buf[0]);
}


void Display::setLUTLongRefresh(){
	writeCMD(SSD1608_WRITE_LUT);
	for(uint32_t i = 0; i<30 ; i++){
		writeDATA(LUT_DATA[i]);
	}
}

void Display::setLUTShortRefresh(){
	writeCMD(SSD1608_WRITE_LUT);
	for(uint32_t i = 0; i<30 ; i++){
		writeDATA(LUT_part1[i]);
	}
}

/******************************************************************************
function:	Perform a reset to the display
******************************************************************************/
void Display::initReset()
{
    this->pinReset->off();
    HAL_Delay(50);
    this->pinReset->on();
    HAL_Delay(50);
}

/******************************************************************************
function:	Check if the display is busy
******************************************************************************/
void Display::lcdChkstatus()
{
	GPIO_PinState busy;
    do
    {
        busy = this->pinBusy->read();
    } while (busy == GPIO_PIN_SET);
    HAL_Delay(10);
}

/******************************************************************************
function:	Display the array (image)
parameter:
    image :   array which contain the picture
******************************************************************************/
void Display::display(uint8_t *image)
{
    //Set ram address to 0
    // Set RAM X address counter
    writeCMD(SSD1608_SET_RAMXCOUNT);
    writeDATA(0x00);
    // Set RAM Y address counter
    writeCMD(SSD1608_SET_RAMYCOUNT);
    writeDATA(0x00);
    writeDATA(0x00);

    writeCMD(SSD1608_WRITE_RAM);
    for(uint32_t i=0;i<5000;i++){
        writeDATA(image[i]);
    }
    writeCMD(SSD1608_DISP_CTRL2);
    writeDATA(0xC7);
    writeCMD(SSD1608_MASTER_ACTIVATE);

    lcdChkstatus();
}

} // namespace ep
