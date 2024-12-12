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

// MOTION CONTROL
// ___________________________

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

// SENSOR CONTROL
// ___________________________

uint32_t readHelios(uint8_t i) {
  helios.setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS + i);
  delay(50);

  float n_samples = 20;
  float n_filter = 5.0;
  uint32_t h = 0;

  for (uint8_t n = 0; n < n_samples; ++n) {
    h = h * (n_filter-1) / n_filter + helios.readADC() / n_filter;
  }

  return h;
}

void tcaSelect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

uint8_t* readTOFs() {
  float n_samples = 20;
  float n_filter = 5.0;
  static uint8_t l_tofs[4] = {0, 0, 0, 0};

  for (uint8_t i = 0; i < 4; ++i) {
    l_tofs[i] = 0;
  }

  for (uint8_t n = 0; n < n_samples; ++n) {
    for (uint8_t i = 0; i < 4; ++i) {
      tcaSelect(i);
      l_tofs[i] = l_tofs[i] * (n_filter-1) / n_filter + tof.readRange() / n_filter;
    }
  }
  
  return l_tofs;
}