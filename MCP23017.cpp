/*************************************************** 
  This is a library for the MCP23017 i2c port expander

  These displays use I2C to communicate
  2 pins are required to interface

  Written by Limor Fried/Ladyada for Adafruit Industries.
    Adafruit invests time and resources providing this open source code, 
    please support Adafruit and open-source hardware by purchasing 
    products from Adafruit!

  Edited by Marc-Andre Ferland/Madrang.

  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <avr/pgmspace.h>
#include "MCP23017.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

// minihelper
static inline void wiresend(uint8_t x) {
#if ARDUINO >= 100
  Wire.write((uint8_t)x);
#else
  Wire.send(x);
#endif
}

static inline uint8_t wirerecv(void) {
#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}

////////////////////////////////////////////////////////////////////////////////

void MCP23017::begin(uint8_t addr) {
  i2caddr = (addr & MCP23017_ADDRES_Msk) | MCP23017_ADDRESS;

  Wire.begin();
  
  // set defaults!
  write16(MCP23017_IODIRA, 0xFFFF); // all inputs
}

void MCP23017::begin(bool a, bool b, bool c) {
  byte addr = 0;
  if(a) {
    addr |= 1 << 0;
  }
  if(b) {
    addr |= 1 << 1;
  }
  if(c) {
    addr |= 1 << 2;
  }
  begin(addr | MCP23017_ADDRESS);
}

void MCP23017::begin(void) {
  begin(0);
}

uint8_t MCP23017::read8(uint8_t addr) {
  Wire.beginTransmission(i2caddr);
  wiresend(addr);
  Wire.endTransmission();

  Wire.requestFrom(i2caddr, (uint8_t)1);
  return wirerecv();
}

uint16_t MCP23017::read16(uint8_t addr) {
  Wire.beginTransmission(i2caddr);
  wiresend(addr);
  Wire.endTransmission();
  
  Wire.requestFrom(i2caddr, (uint8_t)2);
  uint8_t a = wirerecv();
  return (wirerecv() << 8) | a;
}

uint16_t MCP23017::readGPIOAB() {
  return read16(MCP23017_GPIOA);
}

void MCP23017::write8(uint8_t addr, uint8_t val) {
  Wire.beginTransmission(i2caddr);
  wiresend(addr);
  wiresend(val);
  Wire.endTransmission();
}

void MCP23017::write16(uint8_t addr, uint16_t val) {
  Wire.beginTransmission(i2caddr);
  wiresend(addr);
  wiresend(val & 0xFF);
  wiresend(val >> 8);
  Wire.endTransmission();
}

void MCP23017::writeGPIOAB(uint16_t ba) {
  write16(MCP23017_GPIOA, ba);
}

void MCP23017::pinMode(uint8_t p, uint8_t d) {
  // only 16 bits!
  if (p > 15) {
    return;
  }
  
  uint8_t iodiraddr;
  if (p < 8) {
    iodiraddr = MCP23017_IODIRA;
  } else {
    iodiraddr = MCP23017_IODIRB;
    p -= 8;
  }

  // read the current IODIR
  uint8_t iodir = read8(iodiraddr);

  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p; 
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  write8(iodiraddr, iodir);
}

void MCP23017::digitalWrite(uint8_t p, uint8_t d) {
  // only 16 bits!
  if (p > 15) {
    return;
  }
  
  uint8_t olataddr;
  if (p < 8) {
    olataddr = MCP23017_OLATA;
  } else {
    olataddr = MCP23017_OLATB;
    p -= 8;
  }

  // read the current GPIO output latches
  uint8_t gpio = read8(olataddr);

  // set the pin and direction
  if (d == HIGH) {
    gpio |= 1 << p; 
  } else {
    gpio &= ~(1 << p);
  }

  // write the new GPIO
  write8(olataddr, gpio);
}

void MCP23017::pullUp(uint8_t p, uint8_t d) {
  // only 16 bits!
  if (p > 15) {
    return;
  }

  uint8_t gppuaddr;
  if (p < 8)
    gppuaddr = MCP23017_GPPUA;
  else {
    gppuaddr = MCP23017_GPPUB;
    p -= 8;
  }

  // read the current pullup resistor set
  uint8_t gppu = read8(gppuaddr);

  // set the pin and direction
  if (d == HIGH) {
    gppu |= 1 << p; 
  } else {
    gppu &= ~(1 << p);
  }

  // write the new GPIO
  write8(gppuaddr, gppu);
}

uint8_t MCP23017::digitalRead(uint8_t p) {
  // only 16 bits!
  if (p > 15) {
    return 0;
  }

  uint8_t gpioaddr;
  if (p < 8) {
    gpioaddr = MCP23017_GPIOA;
  } else {
    gpioaddr = MCP23017_GPIOB;
    p -= 8;
  }

  // read the current GPIO
  return (read8(gpioaddr) >> p) & 0x1;
}

uint8_t MCP23017::address(void) {
  return i2caddr;
}
