#include "HeliosTestBoard.h"

ActuatorBench :: ActuatorBench(uint8_t dirA, uint8_t stpA, uint8_t dirB, uint8_t stpB, uint8_t dirC, uint8_t stpC, uint8_t dirD, uint8_t stpD, uint8_t en, int spr, float pulleyRadius)
{
  _dirA = dirA;
  _dirB = dirB;
  _dirC = dirC;
  _dirD = dirD;

  _stpA = stpA;
  _stpB = stpB;
  _stpC = stpC;
  _stpD = stpD;

  _en = en;
  _spr = spr;
  _pulleyRadius = pulleyRadius;

  pinMode(_dirA, OUTPUT);
  pinMode(_dirB, OUTPUT);
  pinMode(_dirC, OUTPUT);
  pinMode(_dirD, OUTPUT);

  pinMode(_stpA, OUTPUT);
  pinMode(_stpB, OUTPUT);
  pinMode(_stpC, OUTPUT);
  pinMode(_stpD, OUTPUT);

  pinMode(_en, OUTPUT);  
}

// Execute one step in desired motors. 
void ActuatorBench :: step(int sA, int sB, int sC, int sD, float stepDelay)
{
  // Set direction for each motor:
    digitalWrite(_dirA, REVERSE_A?!sA<0:sA<0);
    digitalWrite(_dirB, REVERSE_B?!sB<0:sB<0);
    digitalWrite(_dirC, REVERSE_C?!sC<0:sC<0);
    digitalWrite(_dirD, REVERSE_D?!sD<0:sD<0);

  // Execute steps:
    digitalWrite(_stpA, sA!=0);
    digitalWrite(_stpB, sB!=0);
    digitalWrite(_stpC, sC!=0);
    digitalWrite(_stpD, sD!=0);
    delayMicroseconds(stepDelay/2);
    
    digitalWrite(_stpA, LOW);
    digitalWrite(_stpB, LOW);
    digitalWrite(_stpC, LOW);
    digitalWrite(_stpD, LOW);
    delayMicroseconds(stepDelay/2);
}