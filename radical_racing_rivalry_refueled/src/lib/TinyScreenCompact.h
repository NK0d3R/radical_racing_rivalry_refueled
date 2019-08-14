// This is a minimal version of TinyScreen.h intended to be used ONLY with
// TinyArcade

/*
TinyScreen.h - Last modified 11 February 2016
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
Written by Ben Rose for TinyCircuits.
The latest version of this library can be found at https://tiny-circuits.com/
*/

#ifndef TinyScreen_h
#define TinyScreen_h

#include <SPI.h>
#include <Arduino.h>

#define USE_DMA             (0)
#if defined USE_DMA && !defined(ARDUINO_ARCH_SAMD)
  #error "Unsupported architecture for DMA"
#endif

// TinyScreen bitDepths
const uint8_t TSBitDepth8  = 0;
const uint8_t TSBitDepth16 = 1;

// TinyScreen Color Modes
const uint8_t TSColorModeBGR = 0;
const uint8_t TSColorModeRGB = 1;
const uint8_t TSP_PIN_DC   = 22;
const uint8_t TSP_PIN_CS   = 38;
const uint8_t TSP_PIN_SHDN = 27;
const uint8_t TSP_PIN_RST  = 26;

const uint8_t TSP_PIN_BT1  = 19;
const uint8_t TSP_PIN_BT2  = 25;
const uint8_t TSP_PIN_BT3  = 30;
const uint8_t TSP_PIN_BT4  = 31;

// GPIO Pins
const uint8_t GPIO_DC =   0x01;
const uint8_t GPIO_CS =   0x02;
const uint8_t GPIO_SHDN = 0x04;
const uint8_t GPIO_RES =  0x08;
const uint8_t GPIO_BTN1 = 0x10;
const uint8_t GPIO_BTN2 = 0x20;
const uint8_t GPIO_BTN3 = 0x40;
const uint8_t GPIO_BTN4 = 0x80;
const uint8_t GPIO_CMD_START = ~(GPIO_CS|GPIO_DC);
const uint8_t GPIO_DATA_START = ~GPIO_CS;
const uint8_t GPIO_TRANSFER_END = GPIO_CS|GPIO_SHDN;

//GPIO Registers
const uint8_t GPIO_RegData = 0x00;
const uint8_t GPIO_RegDir = 0x01;
const uint8_t GPIO_RegPullUp = 0x02;
const uint8_t GPIO_RegInterruptMask = 0x05;
const uint8_t GPIO_RegSenseHigh = 0x06;
const uint8_t GPIO_RegInterruptSource = 0x08;

const uint8_t GPIO_ADDR = 0x20;

class TinyScreenCompact {
  public:
    TinyScreenCompact();
    ~TinyScreenCompact();

    static const uint8_t xMax=95;
    static const uint8_t yMax=63;
    static const uint8_t xRes=xMax + 1;
    static const uint8_t yRes=yMax + 1;
    void start(uint8_t bitDepth = TSBitDepth16,
               uint8_t colorMode = TSColorModeBGR,
               uint8_t flipDisplay = 0,
               uint8_t mirrorDisplay = 0);
    void setBrightness(uint8_t brightness);
    void on();
    void off();
    void clearWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    void clearScreen();
    void writeBuffer(uint8_t *buffer, int count);
    void setX(uint8_t, uint8_t);
    void setY(uint8_t, uint8_t);
    void goTo(uint8_t x, uint8_t y);
    void beginFrame();
    void endFrame();
    uint8_t* getSurface() { return _backBuffer; }
    uint16_t getBufferSize() {
                  return xRes * yRes * (static_cast<uint8_t>(_bitDepth) + 1); }
  private:
    uint8_t* _backBuffer;
    union {
      struct {
        unsigned int  _flipDisplay : 1;
        unsigned int  _mirrorDisplay : 1;
        unsigned int  _bitDepth : 1;
        unsigned int  _colorMode : 1;
      };
      uint8_t  _settings;
    };
    void startData(void);
    void startCommand(void);
    void endTransfer(void);
    void begin();
    void writeRemap(void);
#if USE_DMA
    void initDMA(void);
#endif
};

#endif
