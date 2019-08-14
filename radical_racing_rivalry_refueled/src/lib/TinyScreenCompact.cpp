// This is a minimal, modified version of TinyScreen.cpp intended to be
// used ONLY with TinyArcade

/*
TinyScreen.cpp - Last modified 11 February 2016
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

#include "TinyScreenCompact.h"

#include "pins_arduino.h"
#include "wiring_private.h"
#include <initializer_list>

//These delays are used to allow hardware drawing commands on the SSD1331 to execute
#ifndef TS_USE_DELAY
#define TS_USE_DELAY true
#endif

/*
SPI optimization defines for known architectures
*/

#if defined(ARDUINO_ARCH_AVR)
  #define TS_SPI_SET_DATA_REG(x) SPDR=(x)
#elif defined(ARDUINO_ARCH_SAMD)
  #define TS_SPI_SET_DATA_REG(x) SERCOM4->SPI.DATA.bit.DATA=(x)
#elif defined(ARDUINO_ARCH_ESP8266)
  #define TS_SPI_SET_DATA_REG(x) SPI1W0 = (x); SPI1CMD |= SPIBUSY
#else
  #define TS_SPI_SET_DATA_REG(x) TSSPI->transfer(x)
#endif

#if defined(ARDUINO_ARCH_AVR)
  #define TS_SPI_SEND_WAIT() while(!(SPSR & _BV(SPIF)))
#elif defined(ARDUINO_ARCH_SAMD)
  #define TS_SPI_SEND_WAIT() while(SERCOM4->SPI.INTFLAG.bit.DRE == 0)
#elif defined(ARDUINO_ARCH_ESP8266)
  #define TS_SPI_SEND_WAIT() while(SPI1CMD & SPIBUSY)
#else
  #define TS_SPI_SEND_WAIT() if(0)
#endif

#if defined(ARDUINO_ARCH_SAMD)
  #define TSSPI SPI1
#else
  #error "Unsupported architecture"
#endif

TinyScreenCompact::TinyScreenCompact() {
  _backBuffer = nullptr;
}

TinyScreenCompact::~TinyScreenCompact() {
  delete[] _backBuffer;
}

void TinyScreenCompact::startCommand(void) {
  digitalWrite(TSP_PIN_DC,LOW);
  digitalWrite(TSP_PIN_CS,LOW);
}

void TinyScreenCompact::startData(void) {
  digitalWrite(TSP_PIN_DC,HIGH);
  digitalWrite(TSP_PIN_CS,LOW);
}

void TinyScreenCompact::endTransfer(void) {
  digitalWrite(TSP_PIN_CS,HIGH);
}

void TinyScreenCompact::clearWindow(uint8_t x,
                                    uint8_t y,
                                    uint8_t w,
                                    uint8_t h) {
  // Removing safety checks (what could go wrong?)
  // if (x > xMax || y > yMax) {
  //   return;
  // }
  uint8_t x2 = x + w - 1;
  uint8_t y2 = y + h - 1;
  // if (x2 > xMax) x2 = xMax;
  // if (y2 > yMax) y2 = yMax;

  startCommand();
  TSSPI.transfer(0x25);//clear window
  TSSPI.transfer(x);TSSPI.transfer(y);
  TSSPI.transfer(x2);TSSPI.transfer(y2);
  endTransfer();
#if TS_USE_DELAY
  delayMicroseconds(400);
#endif
}

void TinyScreenCompact::clearScreen() {
  clearWindow(0, 0, xRes, yRes);
}

// Removed limit, max is 15, it will wrap around instead of capping at 15
void TinyScreenCompact::setBrightness(uint8_t brightness) {
  startCommand();
  TSSPI.transfer(0x87);//set master current
  TSSPI.transfer((brightness & 0xf));
  endTransfer();
}

void TinyScreenCompact::on(void) {
  digitalWrite(TSP_PIN_SHDN,HIGH);
  startCommand();//if _externalIO, this will turn boost converter on
  delayMicroseconds(10000);
  TSSPI.transfer(0xAF);//display on
  endTransfer();
}

void TinyScreenCompact::off(void) {
  startCommand();
  TSSPI.transfer(0xAE);//display off
  endTransfer();
  digitalWrite(TSP_PIN_SHDN,LOW);//SHDN
}

void TinyScreenCompact::writeRemap(void) {
  uint8_t remap=(1<<5)|(1<<2);
  if(_flipDisplay)
    remap|=((1<<4)|(1<<1));
  if(_mirrorDisplay)
    remap^=(1<<1);
  if(_bitDepth)
    remap|=(1<<6);
  if(_colorMode)
    remap^=(1<<2);
  startCommand();
  TSSPI.transfer(0xA0);//set remap
  TSSPI.transfer(remap);
  endTransfer();
}

void TinyScreenCompact::begin() {
  //init SPI
  TSSPI.begin();
  TSSPI.setDataMode(SPI_MODE0);//wrong mode, works because we're only writing. this mode is compatible with SD cards.
#if defined(ARDUINO_ARCH_AVR)
  TSSPI.setClockDivider(SPI_CLOCK_DIV2);
#elif defined(ARDUINO_ARCH_SAMD)
  TSSPI.setClockDivider(4);
#endif
  //TinyScreen+, connected directly to IO pins
  for (auto x : { TSP_PIN_SHDN, TSP_PIN_DC, TSP_PIN_CS, TSP_PIN_RST }) {
    pinMode(x, OUTPUT);
  }
  digitalWrite(TSP_PIN_SHDN,LOW);
  for (auto x : { TSP_PIN_DC, TSP_PIN_CS, TSP_PIN_RST }) {
    digitalWrite(x, HIGH);
  }
  for (auto x : { TSP_PIN_BT1, TSP_PIN_BT2, TSP_PIN_BT3, TSP_PIN_BT4 }) {
    pinMode(x, INPUT_PULLUP);
  }

  //reset
  digitalWrite(TSP_PIN_RST,LOW);
  delay(5);
  digitalWrite(TSP_PIN_RST,HIGH);
  delay(10);

  const uint8_t init[32]={0xAE, 0xA1, 0x00, 0xA2, 0x00, 0xA4, 0xA8, 0x3F,
  0xAD, 0x8E, 0xB0, 0x0B, 0xB1, 0x31, 0xB3, 0xF0, 0x8A, 0x64, 0x8B,
  0x78, 0x8C, 0x64, 0xBB, 0x3A, 0xBE, 0x3E, 0x81, 0x91, 0x82, 0x50, 0x83, 0x7D};
  off();
  startCommand();
  for (auto x : init) {
    TSSPI.transfer(x); // TODO: check if transfer(buff) isn't faster
  }
  endTransfer();
  //use libarary functions for remaining init
  setBrightness(10);
  writeRemap();
  //clearScreen();
  on();
}

void TinyScreenCompact::start(uint8_t bitDepth,
                              uint8_t colorMode,
                              uint8_t flipDisplay,
                              uint8_t mirrorDisplay) {
  _bitDepth = (bitDepth & 1);
  _colorMode = (colorMode & 1);
  _flipDisplay = (flipDisplay & 1);
  _mirrorDisplay = (mirrorDisplay & 1);
  if (_backBuffer != nullptr) {
    delete[] _backBuffer;
  }
  _backBuffer = new uint8_t[getBufferSize()];
  begin();

#if USE_DMA
  initDMA();
#endif
}

void TinyScreenCompact::beginFrame() {
  memset(_backBuffer, 0, getBufferSize());
}

void TinyScreenCompact::goTo(uint8_t x, uint8_t y) {
  // Removing safety checks (what could go wrong?)
  // if(x>xMax||y>yMax)return;
  setX(x,xMax);
  setY(y,yMax);
}

void TinyScreenCompact::setX(uint8_t x, uint8_t end) {
  // Removing safety checks (what could go wrong?)
  // if(x>xMax)x=xMax;
  // if(end>xMax)end=xMax;
  startCommand();
  TSSPI.transfer(0x15);//set column
  TSSPI.transfer(x);
  TSSPI.transfer(end);
  endTransfer();
}

void TinyScreenCompact::setY(uint8_t y, uint8_t end) {
  // Removing safety checks (what could go wrong?)
  // if(y>yMax)y=yMax;
  // if(end>yMax)end=yMax;
  startCommand();
  TSSPI.transfer(0x75);//set row
  TSSPI.transfer(y);
  TSSPI.transfer(end);
  endTransfer();
}

#if USE_DMA
typedef struct {
  uint16_t btctrl;
  uint16_t btcnt;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t descaddr;
} dmacdescriptor;

volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));
dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));
dmacdescriptor descriptor __attribute__ ((aligned (16)));

const uint32_t DMAchannel = 0;
volatile uint32_t dmaReady=true;

void DMAC_Handler() {
  // interrupts DMAC_CHINTENCLR_TERR DMAC_CHINTENCLR_TCMPL DMAC_CHINTENCLR_SUSP
  uint8_t active_channel;

  // disable irqs ?
  __disable_irq();
  active_channel = DMAC->INTPEND.reg &
                      DMAC_INTPEND_ID_Msk; // get channel number
  DMAC->CHID.reg = DMAC_CHID_ID(active_channel);
  if(DMAC->CHINTFLAG.reg)dmaReady=true;
  DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL; // clear
  DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
  DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
  __enable_irq();
}

void TinyScreenCompact::writeBuffer(uint8_t *buffer, int count) {
  while(!dmaReady);

  uint32_t temp_CHCTRLB_reg;
  // set up transmit channel
  DMAC->CHID.reg = DMAC_CHID_ID(DMAchannel);
  DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
  DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
  DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << DMAchannel));

  temp_CHCTRLB_reg = DMAC_CHCTRLB_LVL(0) |
                     DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_TX) |
                     DMAC_CHCTRLB_TRIGACT_BEAT;

  DMAC->CHCTRLB.reg = temp_CHCTRLB_reg;
  DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ; // enable interrupts
  descriptor.descaddr = 0;
  descriptor.dstaddr = (uint32_t) &SERCOM4->SPI.DATA.reg;
  descriptor.btcnt =  count;
  descriptor.srcaddr = (uint32_t)buffer;
  descriptor.btctrl =  DMAC_BTCTRL_VALID;
  descriptor.srcaddr += count;
  descriptor.btctrl |= DMAC_BTCTRL_SRCINC;
  memcpy(&descriptor_section[DMAchannel], &descriptor, sizeof(dmacdescriptor));

  dmaReady = false;

  // start channel
  DMAC->CHID.reg = DMAC_CHID_ID(DMAchannel);
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
}

void TinyScreenCompact::initDMA(void) {
  //probably on by default
  PM->AHBMASK.reg |= PM_AHBMASK_DMAC ;
  PM->APBBMASK.reg |= PM_APBBMASK_DMAC ;
  NVIC_EnableIRQ( DMAC_IRQn ) ;

  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;
  DMAC->WRBADDR.reg = (uint32_t)wrb;
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);
}
#else
void TinyScreenCompact::writeBuffer(uint8_t *buffer,int count) {
  uint8_t temp;
  TS_SPI_SET_DATA_REG(buffer[0]);
  for(int j=1;j<count;j++){
    temp=buffer[j];
    TS_SPI_SEND_WAIT();
    TS_SPI_SET_DATA_REG(temp);
  }
  TS_SPI_SEND_WAIT();
}
#endif

void TinyScreenCompact::endFrame() {
  goTo(0, 0);
  startData();
  writeBuffer(_backBuffer, getBufferSize());
#if USE_DMA
	while(!dmaReady);
#endif
  endTransfer();
}
