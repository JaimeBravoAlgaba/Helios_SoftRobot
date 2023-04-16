#include "HeliosTestBoard.h"

HeliosSensor :: HeliosSensor(uint8_t ssPin)
{
  _ssPin = ssPin;
  pinMode(_ssPin, OUTPUT);

  _buffLen = sizeof(HeliosData);
  SPIClass::begin(SCK, MISO, MOSI, _ssPin);
  SPIClass::setClockDivider(SPI_CLOCK_DIV32);
}

HeliosData HeliosSensor :: update()
{  
  for (uint8_t j=0; j<_buffLen; j++)
  {
      digitalWrite(_ssPin, LOW); // enable Slave Select
      delay(READ_DELAY);
      _currReading.data_bytes[j] = (byte)transfer (0x01);
      digitalWrite(_ssPin, HIGH); // disable Slave Select
  }
  return _currReading;
}

uint16_t HeliosSensor :: getReading(uint8_t i)
{
  return _currReading.data[i];
}

void HeliosSensor :: print()
{
    for(uint8_t i=0; i<3; ++i)
    {
      Serial.print(_currReading.data[i]); Serial.print(",");
    }
    Serial.println(_currReading.data[3]);
}