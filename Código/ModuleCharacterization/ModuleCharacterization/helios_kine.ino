#include "Helios.h"

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

float steps2length(long n){
  return n*SEGMENTS_RP*ANGLE_PER_STEP;
}

CoordsPCC euler2pcc(float qx, float qy, float qz)
{
  CoordsPCC coords;
  coords.phi = atan2(qz, qy);
  coords.theta = qy*cos(qx)+qz*sin(qx);
  return coords;
}

CoordsPCC tofs2pcc(float l0, float l1, float l2, float l3){
  float theta_x = atan2((l2-l0), TOFS_DIAM);
  theta_x = (abs(theta_x) < 1E-6)? 1E-3 : theta_x;
  float theta_y = atan2((l3-l1), TOFS_DIAM);
  
  CoordsPCC coords;
  coords.theta = sqrt(theta_x*theta_x + theta_y*theta_y);
  coords.phi = atan2(theta_y, theta_x);
  coords.length = coords.theta * (l0+l1+l2+l3)/4.0 * tan(PI/2.0 - coords.theta);

  return coords;
}