#include "HeliosPanel.h"
# include <std_msgs/msg/multi_array_layout.h>

void printCoordsPCC(CoordsPCC c)
{
  Serial.print(c.theta);
  Serial.print(",");
  Serial.println(c.phi);
}

void buzz(uint8_t n, uint8_t tone, int t_high, int t_low)
{
  for(uint8_t i=0; i<n; ++i)
  {
    analogWrite(BUZZ, tone);
    delay(t_high);
    analogWrite(BUZZ, 0);
    delay(t_low);
  }
}

bool i2cCheckDevice(byte address)
{
  // Returns 1 if a device is found at the given address, 0 otherwise
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}