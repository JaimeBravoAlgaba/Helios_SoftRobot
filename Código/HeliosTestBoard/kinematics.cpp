#include "kinematics.h"
Section::Section(int N, float L, float rc, float rp)
{
  _nSegments = N;
  _length = L;
  _rc = rc;
  _rp = rp;
  _delta[0] = 0;
  _delta[1] = M_PI/2.0;
  _delta[2] = M_PI;
  _delta[3] = 3*M_PI/2.0;
}

float Section::cableIKine(float theta, float phi, uint8_t i)
{
  return 2*sin(theta/(2.0*_nSegments))*(_length*_nSegments/theta-_rc*sin(phi+_delta[i]));
}

int Section::length2steps(float l)
{
  return l/(_rp*ANGLE_PER_STEP);
}