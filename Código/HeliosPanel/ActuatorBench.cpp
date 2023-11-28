#include "HeliosPanel.h"

float cableLengths[3][4] = {{0,0,0},{0,0,0},{0,0,0}};
float cableOffsets[3] = {0,0,0}; // REVIEW!!!!

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
}

float cableIKine(CoordsPCC coords, uint8_t i)
{
  return 2*sin(coords.theta/(2.0*float(SEGMENTS_NUM)))*(SEGMENTS_LEN*float(SEGMENTS_NUM)/coords.theta+SEGMENTS_RC*sin(coords.phi+cableOffsets[i]))*1.115;
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
    digitalWrite(D0, REVERSE_0?!s0<0:s0<0);
    digitalWrite(D1, REVERSE_1?!s1<0:s1<0);
    digitalWrite(D2, REVERSE_2?!s2<0:s2<0);
    digitalWrite(D3, REVERSE_3?!s3<0:s3<0);

  // Execute steps:
    digitalWrite(S0, s0!=0);
    digitalWrite(S1, s1!=0);
    digitalWrite(S2, s2!=0);
    digitalWrite(S3, s3!=0);
    delayMicroseconds(stepDelay/2);
    
  enableSection(SEC_ALL);
    
    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    delayMicroseconds(stepDelay/2);
}

void moveSection(uint8_t sec, CoordsPCC ref)
{ 
  int dn[4];
  for(uint8_t i=0; i<4; ++i)
  {
    float aux =  cableIKine(ref, i);
    dn[i] = length2steps(aux-cableLengths[sec][i]);
    cableLengths[sec][i] = aux;
  }

  int N = max({abs(dn[0]), abs(dn[1]), abs(dn[2]), abs(dn[3])});
  float v[4] = {dn[0]/(N*STEP_DELAY), dn[1]/(N*STEP_DELAY), dn[2]/(N*STEP_DELAY), dn[3]/(N*STEP_DELAY)};
  float r[4] = {0, 0, 0, 0};
  int   s[4] = {0, 0, 0, 0};

  for (int i=0; i<N; ++i)
  {
    for(uint8_t i=0; i<4; ++i)
    {
      r[i] = r[i] + v[i]*STEP_DELAY;
      s[i] = floor(r[i]);
      r[i] = r[i] - float(s[i]);
    }
    stepSection(sec, s[0], s[1], s[2], s[3], STEP_DELAY);
  }
}