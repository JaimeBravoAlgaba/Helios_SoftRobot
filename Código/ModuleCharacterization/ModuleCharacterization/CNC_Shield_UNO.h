#ifndef __CNC_SHIELD_UNO__ 
# define __CNC_SHIELD_UNO__


  #define EN_DEBUG 1
  inline void print_info(String txt){
    if(EN_DEBUG)
      Serial.print(String(txt));
  }

  const uint8_t X_MOT = 0;
  const uint8_t Y_MOT = 1;
  const uint8_t Z_MOT = 2;
  const uint8_t A_MOT = 3;

  const uint8_t X_REVERSE = 0;
  const uint8_t Y_REVERSE = 0;
  const uint8_t Z_REVERSE = 0;
  const uint8_t A_REVERSE = 0;

  // PINOUT
  const uint8_t X_STEP = 2;
  const uint8_t Y_STEP = 3;
  const uint8_t Z_STEP = 4;

  const uint8_t X_DIR = 5;
  const uint8_t Y_DIR = 6;
  const uint8_t Z_DIR = 7;

  const uint8_t MOT_EN = 8;
  
  const uint8_t X_STOP = 9;
  const uint8_t Y_STOP = 10;
  const uint8_t Z_STOP = 11;

  const uint8_t A_STEP = 12;
  const uint8_t A_DIR = 13;
  
  const uint8_t ABORT = A0;
  const uint8_t HOLD = A1;
  const uint8_t RESUME = A2;
  const uint8_t COOL_EN = A3;

  // TIMING
  const int STEP_DELAY_HIGH = 1; // Microseconds
  const int STEP_DELAY_LOW = 1; // Microseconds

  // DIMENSIONS
  const float SEGMENTS_NUM = 1.0;     // Number of segments in a section
  const float SEGMENTS_LEN = 0.0445;    // Length [m] of a segment
  const float SEGMENTS_RC  = 0.0225;  // Radius [m] of the cable distribution circunference
  const float SEGMENTS_RP  = 0.005;   // Radius [m] of the actuator pulley
  const float TOFS_DIAM = 100.0; // Distance [mm] between opposite TOFs

  const float GEAR_RATIO = 20;
  const float STEPS_PER_REVOLUTION = (200*32*GEAR_RATIO);
  const float ANGLE_PER_STEP = 2*M_PI/STEPS_PER_REVOLUTION;

  // Data structure to manage the coordinates of a PCC section.
  struct CoordsPCC
  {
    float length;
    float theta;
    float phi;
  };

  // MOTION
  void setupCNC();
  void step(const uint8_t mot, long n);
  void stepParallel(int dn[4]);
  float cableIKine(CoordsPCC coords, uint8_t i);
  long length2steps(float l);
#endif