#include "HeliosPanel.h"

SensorData currReading[4] = {0,0,0,0};
uint8_t buffLen = sizeof(currReading);

void initSensor()
{
  Wire.begin();
}

void updateSensor()
{
  Wire.requestFrom(I2C_ADDR_HS0, buffLen);

  uint8_t index = 0;
  byte* pointer = (byte*)currReading;
  while (Wire.available())
  {
    *(pointer + index) = (byte)Wire.read();
    index++;
  }
}

SensorData getSensorReading(uint8_t i)
{
  return currReading[i];
}

void printSensor()
{
    Serial.print(0); Serial.print(",");
    for(uint8_t i=0; i<3; ++i)
    {
      Serial.print(currReading[i]); Serial.print(",");
    }
    Serial.println(currReading[3]);
}