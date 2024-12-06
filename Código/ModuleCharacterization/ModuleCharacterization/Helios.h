#ifndef __HELIOS__
  #define __HELIOS__

  #include <Adafruit_Sensor.h>
  #include <Adafruit_BNO055.h>
  #include <utility/imumaths.h>
  #include <EEPROM.h>
  #include <Adafruit_VL6180X.h>
  #include <SparkFun_ADS122C04_ADC_Arduino_Library.h>

  typedef SFE_ADS122C04 HeliosSensor;

  const float tof_diam = 100.0; // Distance between opposite TOFs [mm]

  // Data structure to manage the coordinates of a PCC section.
  struct CoordsPCC
  {
    float length;
    float theta;
    float phi;
  };

  float cableIKine(CoordsPCC coords, uint8_t i);
  long length2steps(float l);

  CoordsPCC euler2pcc(float qx, float qy, float qz);
  CoordsPCC tofs2pcc(float l0, float l1, float l2, float l3);
#endif