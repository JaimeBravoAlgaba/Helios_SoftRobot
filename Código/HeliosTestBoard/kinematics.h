#pragma once
  #include <math.h>
  #define STEPS_PER_REVOLUTION 7000
  #define ANGLE_PER_STEP 2*M_PI/(STEPS_PER_REVOLUTION*1.0)

  class Section
  {
    public:
      Section(int N, float L, float rc, float rp);

      float cableIKine(float theta, float phi, uint8_t i);
      int length2steps(float l);

    private:
      int _nSegments;         // Number of segments.
      float _length;           // Segment length.
      float _rc;          // Cable disposition radius.
      float _rp;          // Pulley radius.
      float _delta[4];  // Cable offset angle.
  };