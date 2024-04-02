#include "HeliosPanel.h"

// KINE VARIABLES
float cableLengths[N_SECTIONS][N_CABLES] = {0};               // Length of each cable in each section

void initActuator()
{
  pinMode(EN0, OUTPUT); 
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT); 

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);

  pinMode(BUZZ, OUTPUT);
}

float cableIKine(CoordsPCC coords, uint8_t sec, uint8_t cable)
{
  // Asign to matrix the mounting matrix, depending os the demanded section
  float* matrix;
  float l = 0;

  switch(sec)
  {
    case SEC0:
      return -SEGMENTS_RC*coords.theta*(mountMatrix_0[cable][0]*cos(coords.phi) + mountMatrix_0[cable][1]*sin(coords.theta));
    
    case SEC1:
      return -SEGMENTS_RC*coords.theta*(mountMatrix_1[cable][0]*cos(coords.phi) + mountMatrix_1[cable][1]*sin(coords.theta));

    case SEC2:
      return -SEGMENTS_RC*coords.theta*(mountMatrix_2[cable][0]*cos(coords.phi) + mountMatrix_2[cable][1]*sin(coords.theta));
    
    default:
      return 0;
  }
}

int length2steps(float l)
{
  return l/(SEGMENTS_RP*ANGLE_PER_STEP);
}

void enableSection(uint8_t sec)
{
  switch (sec)
  {
    case 0:
      digitalWrite(EN0, LOW);
      break;

    case 1:
      digitalWrite(EN1, LOW);
      break;

    case 2:
      digitalWrite(EN2, LOW);
      break;

    default:
      digitalWrite(EN0, LOW);
      digitalWrite(EN1, LOW);
      digitalWrite(EN2, LOW);
      break;
  }
}

void disableSection(uint8_t sec)
{
  switch (sec)
  {
    case 0:
      digitalWrite(EN0, HIGH);
      break;

    case 1:
      digitalWrite(EN1, HIGH);
      break;

    case 2:
      digitalWrite(EN2, HIGH);
      break;

    default:
      digitalWrite(EN0, HIGH);
      digitalWrite(EN1, HIGH);
      digitalWrite(EN2, HIGH);
      break;
  }
}

// Execute one step in desired motors. 
void stepSection(uint8_t sec, int s0, int s1, int s2, int s3, float stepDelay)
{
  disableSection(SEC_ALL);
  enableSection(sec);
  // Set direction for each motor:
    digitalWrite(D0, REVERSE_0?s0<0:s0>=0);
    digitalWrite(D1, REVERSE_1?s1<0:s1>=0);
    digitalWrite(D2, REVERSE_2?s2<0:s2>=0);
    digitalWrite(D3, REVERSE_3?s3<0:s3>=0);

  // Execute steps:
    digitalWrite(S0, s0!=0);
    digitalWrite(S1, s1!=0);
    digitalWrite(S2, s2!=0);
    digitalWrite(S3, s3!=0);
    delayMicroseconds(stepDelay*STEP_RATIO);
    
    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    delayMicroseconds(stepDelay*(1-STEP_RATIO));
  disableSection(sec);
}

void moveSection(uint8_t sec, CoordsPCC ref)
{ 
  ref.theta = (ref.theta==0)?ref.theta=0.001:ref.theta;
  int dn[4];
  for(uint8_t i=0; i<4; ++i)
  {
    float aux =  cableIKine(ref, sec, i);
    dn[i] = length2steps(aux-cableLengths[sec][i]);
    cableLengths[sec][i] = aux;
  }

  Serial.print(dn[0]); Serial.print(",");
  Serial.print(dn[1]); Serial.print(",");
  Serial.print(dn[2]); Serial.print(",");
  Serial.println(dn[3]);

  float N = max({abs(dn[0]), abs(dn[1]), abs(dn[2]), abs(dn[3])});
  float r[4] = {0, 0, 0, 0};
  int   s[4] = {0, 0, 0, 0};

  for (int i=0; i<N; ++i)
  {
    for(uint8_t i=0; i<4; ++i)
    {
      r[i] = r[i] + float(dn[i])/N;
      s[i] = floor(r[i]);
      r[i] = r[i] - float(s[i]);
    }

    stepSection(sec, s[0], s[1], s[2], s[3], STEP_DELAY);
  }
}