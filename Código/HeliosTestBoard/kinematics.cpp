#include "HeliosTestBoard.h"

ContinuumSection::ContinuumSection(int N, float L, float rc, float rp)
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

float ContinuumSection::cableIKine(CoordsPCC coords, uint8_t i)
{
  return 2*sin(coords.theta/(2.0*_nSegments))*(_length*_nSegments/coords.theta-_rc*sin(coords.phi+_delta[i]));
}

int ContinuumSection::length2steps(float l)
{
  return l/(_rp*ANGLE_PER_STEP);
}