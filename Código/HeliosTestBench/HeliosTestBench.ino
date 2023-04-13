#define   STP_EN  8
#define   MA_DIR  5 //Stepper shield: X_DIR
#define   MB_DIR  7 //Stepper shield: Y_DIR
#define   MC_DIR  7 //Stepper shield: Z_DIR

#define   MA_STP  2 //Stepper shield: X_STP
#define   MB_STP  3 //Stepper shield: Y_STP
#define   MC_STP  4 //Stepper shield: Z_STP

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

void setup()
{
  testBenchInit();
  digitalWrite(STP_EN, LOW);
  Serial.begin(9600);
}

void loop()
{ 
  for(int i=0; i<2000; ++i)
  {
    moveStepper('A', 1, 1, 75);
    moveStepper('C', 1, 0, 75);
  }
  delay(1000);

  for(int i=0; i<2000; ++i)
  {
    moveStepper('A', 1, 0, 75);
    moveStepper('C', 1, 1, 75);
  }
  delay(1000);
}
