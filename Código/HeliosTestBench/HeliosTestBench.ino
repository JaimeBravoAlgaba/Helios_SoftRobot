#define F0 A3
#define F1 A0
#define F2 A1
#define F3 A2
#define STEP_EN 8

#define  MA_DIR 5
#define MA_STEP 2
#define  MC_DIR 7
#define MC_STEP 4

const int led_on_time = 50;

struct measurement
{
  int f0 = 0, f1 = 0, f2 = 0, f3 = 0;
  int mean = 0; 
};

measurement data;

void heliosInit()
{
  pinMode(F0, INPUT);
  pinMode(F1, INPUT);
  pinMode(F2, INPUT);
  pinMode(F3, INPUT);
  pinMode(STEP_EN, OUTPUT);
  
  pinMode(MA_DIR, OUTPUT);
  pinMode(MA_STEP, OUTPUT);
  pinMode(MC_DIR, OUTPUT);
  pinMode(MC_STEP, OUTPUT);
}

void moveStepper(char mot, int nSteps, int dir, int stepDelay)
{
   int dirPin, stepPin;

   switch(mot)
   {
       case 'A': 
        dirPin = MA_DIR;
        stepPin = MA_STEP;
       break;
       case 'C': 
        dirPin = MC_DIR;
        stepPin = MC_STEP;
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

measurement heliosMeasure()
{
  measurement data_aux = data;
  delay(led_on_time);
  data_aux.f0 = analogRead(F0);
  data_aux.f1 = analogRead(F1);
  data_aux.f2 = analogRead(F2);
  data_aux.f3 = analogRead(F3);

  data_aux.mean = (data_aux.f0 + data_aux.f1 + data_aux.f2 + data_aux.f3)/4.0;

  return data_aux;
}

void setup()
{
  heliosInit();
  digitalWrite(STEP_EN, LOW);
  Serial.begin(9600);
}

void loop()
{ 
//  data = heliosMeasure();
//  
//  Serial.print(data.f0-data.mean);
//  Serial.print(',');
//  Serial.print(data.f1-data.mean);
//  Serial.print(',');  
//  Serial.print(data.f2-data.mean);
//  Serial.print(',');  
//  Serial.print(data.f3-data.mean);
//  Serial.print(',');  
//  Serial.println(data.mean);
//
//  delay(100);

for(int i=0; i<2000; ++i)
{
  moveStepper('A', 1, 1, 75);
  moveStepper('C', 1, 0, 75);
}
delay(1000);
for(int i=0; i<5000; ++i)
{
  moveStepper('A', 1, 0, 75);
  moveStepper('C', 1, 1, 75);
}
delay(1000);for(int i=0; i<3000; ++i)
{
  moveStepper('A', 1, 1, 75);
  moveStepper('C', 1, 0, 75);
}
}
