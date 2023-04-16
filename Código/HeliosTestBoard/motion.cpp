#include "HeliosTestBoard.h"
#include <Arduino.h>

void testBenchInit()
{
  pinMode(EN_MOT, OUTPUT);  

  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(DIR_C, OUTPUT);
  pinMode(DIR_D, OUTPUT);

  pinMode(STP_A, OUTPUT);
  pinMode(STP_B, OUTPUT);
  pinMode(STP_C, OUTPUT);
  pinMode(STP_D, OUTPUT);

  pinMode(SS1, OUTPUT);
}

void moveStepper(char mot, int nSteps, int dir, int stepDelay)
{
   int dirPin, stepPin;

   switch(mot)
   {
       case 'A': 
        dirPin  = DIR_A;
        stepPin = STP_A;
       break;
       
       case 'C': 
        dirPin  = DIR_C;
        stepPin = STP_C;
       break;
   }
   
   //Activar una direccion y fijar la velocidad con stepDelay
   digitalWrite(dirPin, dir);
   
   // Giramos 200 pulsos para hacer una vuelta completa
   for (int x = 0; x < nSteps * 1; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);

      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
   }  
}

void disableMotors()
{
  digitalWrite(EN_MOT, HIGH);
  digitalWrite(STP_A, LOW);
  digitalWrite(DIR_A, LOW);
  digitalWrite(STP_B, LOW);
  digitalWrite(DIR_B, LOW);
  digitalWrite(STP_C, LOW);
  digitalWrite(DIR_C, LOW);
  digitalWrite(STP_D, LOW);
  digitalWrite(DIR_D, LOW);
}

void enableMotors () {digitalWrite(EN_MOT, LOW);}