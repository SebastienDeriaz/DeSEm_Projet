#ifndef EP_DISPLAY_H
#define EP_DISPLAY_H

#include "stdint.h"
#include "mcu/mcu.h"
#include "lutarray.h"
#include "hal/gpo.h"
#include "hal/gpi.h"

#define WIDTH                       200
#define HEIGHT                      200
#define TOTAL_BYTES                 5000

#define SSD1608_DRIVER_CONTROL 0x01
#define SSD1608_GATE_VOLTAGE 0x03
#define SSD1608_SOURCE_VOLTAGE 0x04
#define SSD1608_DISPLAY_CONTROL 0x07
#define SSD1608_NON_OVERLAP 0x0B
#define SSD1608_BOOSTER_SOFT_START 0x0C
#define SSD1608_GATE_SCAN_START 0x0F
#define SSD1608_DEEP_SLEEP 0x10
#define SSD1608_DATA_MODE 0x11
#define SSD1608_SW_RESET 0x12
#define SSD1608_TEMP_WRITE 0x1A
#define SSD1608_TEMP_READ 0x1B
#define SSD1608_TEMP_CONTROL 0x1C
#define SSD1608_TEMP_LOAD 0x1D
#define SSD1608_MASTER_ACTIVATE 0x20
#define SSD1608_DISP_CTRL1 0x21
#define SSD1608_DISP_CTRL2 0x22
#define SSD1608_WRITE_RAM 0x24
#define SSD1608_READ_RAM 0x25
#define SSD1608_VCOM_SENSE 0x28
#define SSD1608_VCOM_DURATION 0x29
#define SSD1608_WRITE_VCOM 0x2C
#define SSD1608_READ_OTP 0x2D
#define SSD1608_WRITE_LUT 0x32
#define SSD1608_WRITE_DUMMY 0x3A
#define SSD1608_WRITE_GATELINE 0x3B
#define SSD1608_WRITE_BORDER 0x3C
#define SSD1608_SET_RAMXPOS 0x44
#define SSD1608_SET_RAMYPOS 0x45
#define SSD1608_SET_RAMXCOUNT 0x4E
#define SSD1608_SET_RAMYCOUNT 0x4F
#define SSD1608_NOP 0xFF

namespace ep {
/**
 * @class Display
 * @brief This class is used to perform the communication between the uP and the chip controlling the display (here SSD1608), the communication is performed with the SPI protocol
 * For that, we need to write to the RAM of the chip the value of each pixel (0 ->black, 1 ->white), to draw something special, like a line or a circle, the paint class will help you, here
 * we only take an array containing the value of each pixel.
 */
class Display
{
public:
    Display();
    ~Display() = default;
    void initialize(SPI_HandleTypeDef * hspi2, Gpo * pinDC, Gpi * pinBusy, Gpo * pinReset);
    static Display * instance();

    //Function configuration
    void writeCMD(uint8_t command);
    void writeDATA(uint8_t command);
    void setLUTLongRefresh();
    void setLUTShortRefresh();
    void init();
    void display(uint8_t *image);

protected:
    static Display * pInstance;

private:
    SPI_HandleTypeDef * spi;
    Gpo * pinDC;                ///< Data command pin for SSD1608.
    Gpi * pinBusy;              ///< Busy pin which indicates if chip SSD1608 is free or not.
    Gpo * pinReset;             ///< Reset pin for the display.
    
private:
    void SPI_Write(unsigned char command);
    void initReset();
    void lcdChkstatus();
};

} // namespace ep
#endif // EP_DISPLAY_H
