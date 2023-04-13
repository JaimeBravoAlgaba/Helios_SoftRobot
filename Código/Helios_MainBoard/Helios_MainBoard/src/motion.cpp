#include <Arduino.h>
#include <motion.h>

void testBenchInit()
{
  pinMode(STP_EN, OUTPUT);  
  pinMode(MA_DIR, OUTPUT);
  pinMode(MA_STP, OUTPUT);
  pinMode(MC_DIR, OUTPUT);
  pinMode(MC_STP, OUTPUT);
}

void moveStepper(char mot, int nSteps, int dir, int stepDelay)
{
   int dirPin, stepPin;

   switch(mot)
   {
       case 'A': 
        dirPin  = MA_DIR;
        stepPin = MA_STP;
       break;
       
       case 'C': 
        dirPin  = MC_DIR;
        stepPin = MC_STP;
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