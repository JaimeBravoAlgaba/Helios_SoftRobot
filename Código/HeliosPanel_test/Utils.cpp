#include "HeliosPanel.h"

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