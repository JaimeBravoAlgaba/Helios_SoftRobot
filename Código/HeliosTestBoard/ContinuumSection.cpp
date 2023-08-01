#include "HeliosTestBoard.h"

ContinuumSection::ContinuumSection(ActuatorBench actuator, int N, float L, float rc, float rp)
{
  _actuator = actuator;
  _nSegments = N;
  _length = L;
  _rc = rc;
  _rp = rp;
  _offset[0] = 0;
  _offset[1] = M_PI/2.0;
  _offset[2] = M_PI;
  _offset[3] = 3*M_PI/2.0;

  for(uint8_t i=0; i<4; ++i)
    _cableLengths[i] = _length;
}

float ContinuumSection::cableIKine(CoordsPCC coords, uint8_t i)
{
  return 2*sin(coords.theta/(2.0*float(_nSegments)))*(_length*float(_nSegments)/coords.theta+_rc*sin(coords.phi+_offset[i]))*1.115;
}

int ContinuumSection::length2steps(float l)
{
  return l/(_rp*ANGLE_PER_STEP);
}

void ContinuumSection::move(CoordsPCC ref)
{ 
  int dn[4];
  for(uint8_t i=0; i<4; ++i)
  {
    float aux =  cableIKine(ref, i);
    dn[i] = length2steps(aux-_cableLengths[i]);
    _cableLengths[i] = aux;
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
    _actuator.step(s[0], s[1], s[2], s[3], STEP_DELAY);
  }
}