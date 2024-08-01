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
void stepSection(uint8_t sec, const int s[N_CABLES], float stepDelay)
{
  //disableSection(SEC_ALL);
  enableSection(sec);
  // Set direction for each motor:
    digitalWrite(D0, REVERSE_0?s[0]<0:s[0]>=0);
    digitalWrite(D1, REVERSE_1?s[1]<0:s[1]>=0);
    digitalWrite(D2, REVERSE_2?s[2]<0:s[2]>=0);
    digitalWrite(D3, REVERSE_3?s[3]<0:s[3]>=0);

  // Execute steps:
    digitalWrite(S0, s[0]!=0);
    digitalWrite(S1, s[1]!=0);
    digitalWrite(S2, s[2]!=0);
    digitalWrite(S3, s[3]!=0);
    delayMicroseconds(stepDelay*STEP_RATIO);
    
    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    delayMicroseconds(stepDelay*(1-STEP_RATIO));
  disableSection(sec);
}

void moveSection(uint8_t sec, const float ref[N_CABLES])
{ 
  int dn[4];
  for(uint8_t i=0; i<4; ++i)
    dn[i] = length2steps(ref[i]);

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

    stepSection(sec, s, STEP_DELAY);
  }
}

void moveRobot(const float ref[N_SECTIONS*N_CABLES])
{ 
  int dn[N_SECTIONS*N_CABLES];
  float N = 0;
  for(uint8_t i=0; i<N_SECTIONS*N_CABLES; ++i)
  {
    dn[i] = length2steps(ref[i]);
    N = (N>abs(dn[i]))?N:abs(dn[i]);
  }

  float r[N_SECTIONS*N_CABLES] = {0};
  int   s[N_SECTIONS*N_CABLES] = {0};

  for (int i=0; i<N; ++i)
  {
    for(uint8_t i=0; i<4; ++i)
    {
      r[i] = r[i] + float(dn[i])/N;
      s[i] = floor(r[i]);
      r[i] = r[i] - float(s[i]);
    }
    
    for(uint8_t j=0; j<N_SECTIONS; ++j)
    {
      int ss[4] = {s[4*j], s[4*j+1], s[4*j+2], s[4*j+3]};
      stepSection(SEC0+j, ss, STEP_DELAY);
    }
  }
}