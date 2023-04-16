#include "HeliosTestBoard.h"

Actuator::Actuator(uint8_t dir, uint8_t stp, uint8_t en, int spr, float pulleyRadius)
{
  _dir = dir;
  _stp = stp;
  _en = en;
  _spr = spr;
  _pulleyRadius = pulleyRadius;
  _reverse = false;

  pinMode(_dir, OUTPUT);
  pinMode(_stp, OUTPUT);
  pinMode(_en, OUTPUT);  
}

void Actuator::step(int steps, uint32_t stepDelay)
{ 
  uint8_t dir = (steps<0);
  dir = (_reverse) ? !dir : dir;

  // Activate direction:
  digitalWrite(_dir, dir);
  
  int n = abs(steps);
  // Execute steps:
  for (int x = 0; x < n; x++) {
    digitalWrite(_stp, HIGH);
    delayMicroseconds(stepDelay/2);
    digitalWrite(_stp, LOW);
    delayMicroseconds(stepDelay/2);
  }
}