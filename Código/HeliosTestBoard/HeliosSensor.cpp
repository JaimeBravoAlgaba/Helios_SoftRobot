#include "HeliosTestBoard.h"

HeliosSensor :: HeliosSensor()
{
  _buffLen = sizeof(_currReading);
  Wire.begin();
}

void HeliosSensor :: update()
{
  Wire.requestFrom(I2C_ADDR_HS0, _buffLen);

  uint8_t index = 0;
  byte* pointer = (byte*)_currReading;
  while (Wire.available())
  {
    *(pointer + index) = (byte)Wire.read();
    index++;
  }
}

SensorData HeliosSensor :: getReading(uint8_t i)
{
  return _currReading[i];
}

void HeliosSensor :: print()
{
    Serial.print(0); Serial.print(",");
    for(uint8_t i=0; i<3; ++i)
    {
      Serial.print(_currReading[i]); Serial.print(",");
    }
    Serial.println(_currReading[3]);
}