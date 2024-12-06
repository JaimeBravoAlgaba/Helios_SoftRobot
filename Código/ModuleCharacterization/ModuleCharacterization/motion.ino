#include "CNC_Shield_UNO.h"

void setupCNC(){
  pinMode(X_STEP, OUTPUT);
  pinMode(Y_STEP, OUTPUT);
  pinMode(Z_STEP, OUTPUT);
  pinMode(A_STEP, OUTPUT);

  pinMode(X_DIR, OUTPUT);
  pinMode(Y_DIR, OUTPUT);
  pinMode(Z_DIR, OUTPUT);
  pinMode(A_DIR, OUTPUT);

  pinMode(MOT_EN, OUTPUT);

  pinMode(X_STOP, INPUT_PULLUP);
  pinMode(Y_STOP, INPUT_PULLUP);
  pinMode(Z_STOP, INPUT_PULLUP);
  
  //...
}

void step(const uint8_t mot, long n) {
  uint8_t dir = (n<0);
  n = abs(n);
  uint8_t stepPin, dirPin;

  switch(mot){
    case X_MOT:
      stepPin = X_STEP;
      dirPin = X_DIR;
      dir = dir ^ X_REVERSE;
      break;

    case Y_MOT:
      stepPin = Y_STEP;
      dirPin = Y_DIR;
      dir = dir ^ Y_REVERSE;
      break;

    case Z_MOT:
      stepPin = Z_STEP;
      dirPin = Z_DIR;
      dir = dir ^ Z_REVERSE;
      break;

    case A_MOT:
      stepPin = A_STEP;
      dirPin = A_DIR;
      dir = dir ^ A_REVERSE;
      break;

    default:
      return -1;
    }
  digitalWrite(dirPin, dir);

  while(n){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(STEP_DELAY_HIGH);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(STEP_DELAY_LOW);
    --n;
  }
}

void stepParallel(long dn[4])
{ 
  long aux = max(abs(dn[0]), abs(dn[1]));
  long N = max(abs(dn[2]), abs(dn[3]));
  N = max(N,aux);
  float r[4] = {0, 0, 0, 0};
  int   s[4] = {0, 0, 0, 0};

  for (long i=0; i<N; ++i) {
    for(uint8_t j=0; j<4; ++j) {
      r[j] = r[j] + float(dn[j])/(float)N;
      s[j] = floor(r[j]);
      r[j] = r[j] - float(s[j]);

      step(j, s[j]);
    }
  }
}

float cableOffsets[4] = {0, PI/2.0, PI, -PI/2.0};

float cableIKine(CoordsPCC coords, uint8_t i)
{
  coords.phi = (coords.theta<0)?coords.phi+PI:coords.phi;
  coords.theta = (abs(coords.theta)<1E-4)?0.0001:abs(coords.theta);

  return 2*sin(coords.theta/(2.0*SEGMENTS_NUM))*(coords.length*SEGMENTS_NUM/coords.theta - SEGMENTS_RC*sin(coords.phi+cableOffsets[i]));
}

long length2steps(float l){
  return ceil(l/(SEGMENTS_RP*ANGLE_PER_STEP));
}