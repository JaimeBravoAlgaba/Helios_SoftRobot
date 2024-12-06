#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <EEPROM.h>
#include <Adafruit_VL6180X.h>
#include <SparkFun_ADS122C04_ADC_Arduino_Library.h> // Modded library for compatibility with Helios project
#include "CNC_Shield_UNO.h"

#define HELIOS_ADDR 0x4A
#define IMU_0_ADDR 0x08
#define TCA_ADDR 0x70

SFE_ADS122C04 heliosSensor;
Adafruit_VL6180X tof = Adafruit_VL6180X();

uint32_t h[4] = {0, 0, 0, 0}; // Helios module measurement 
float cableLengths[4] = {SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN};
sensors_event_t orientationData;

CoordsPCC coords;
CoordsPCC coords_meas;
float qx_0 = 0, qy_0 = 0, qz_0 = 0;
float l0 = 0, l1 = 0, l2 = 0, l3 = 0;

bool succ_init = true;

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

uint32_t readSensor(uint8_t i)
{
      heliosSensor.setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS + i);
      delay(50);
      return heliosSensor.readADC(); 
}

void tcaSelect(uint8_t i)
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void readTOFs()
{
  tcaSelect(0);
  l0 = tof.readRange();
  
  tcaSelect(1);
  l1 = tof.readRange();
  
  tcaSelect(2);
  l2 = tof.readRange();
  
  tcaSelect(3);
  l3 = tof.readRange();
}

void move(CoordsPCC c)
{
  float l_ref[4] = {0, 0, 0, 0};
  long dn[4] = {0, 0, 0, 0};
  
  for(uint8_t i=0; i<4; ++i)
  {
    l_ref[i] = cableIKine(c, i);
    dn[i] = length2steps(l_ref[i]-cableLengths[i]);
    cableLengths[i] = l_ref[i];
  }
  stepParallel(dn);
}

void calibrateCables()
{
  print_info("Starting calibration... ");
  // 1. De-tension
  long n = length2steps(0.007); // Lengthen the cables by 5mm
  long dn[4] = {n, n, n, n};
  stepParallel(dn);
  delay(500);

  for(uint8_t i=0; i<4; ++i)
  {
    dn[i] = 0;
  }

  // 2. Tension loop
  n = length2steps(-0.00025); // Shorten ecah cable by 1mm per cycle
  for (uint8_t i=0; i<4; ++i)
  {
    h[i] = readSensor(i);
    //print_info(h[i]); print_info(",");
    int thr = 0.007*h[i];
    uint32_t u = h[i];
    dn[i] = n;
    
    while(abs((int)h[i] - (int)u) < thr)
    {
      stepParallel(dn);
      u = readSensor(i);
    }
    cableLengths[i] = SEGMENTS_LEN;
    dn[i] = 0;
    delay(100);
  }

  print_info("OK\n");
}

void printData()
{
  /*print_info(String(coords_meas.theta * 180/PI)); print_info(",");
  print_info(String(coords_meas.phi * 180/PI)); print_info(",");
  print_info(String(coords_meas.length)); print_info(",");*/
  print_info(String(l0)); print_info(",");
  print_info(String(l1)); print_info(",");
  print_info(String(l2)); print_info(",");
  print_info(String(l3)); print_info(",");

  print_info(String(h[0])); print_info(",");
  print_info(String(h[1])); print_info(",");
  print_info(String(h[2])); print_info(",");
  print_info(String(h[3])); print_info("\n");
}

void setup()
{
  setupCNC();

  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for serial port to open
  Wire.begin();

  // HELIOS INIT
  print_info("Initializing Helios module on address ");
  print_info(String(HELIOS_ADDR)); print_info("\n");
    while(!heliosSensor.begin(HELIOS_ADDR)){
      print_info(F("Helios Module not detected. Retrying...\n"));
      delay(500);
    }

    heliosSensor.configureADCmode(ADS122C04_RAW_MODE);
    heliosSensor.setGain(ADS122C04_GAIN_128);
    heliosSensor.setConversionMode(ADS122C04_CONVERSION_MODE_CONTINUOUS);
    heliosSensor.start();

    print_info("OK\n");

    // TCA9548A Initialization
    print_info("Initializing TCA9548A with address ");
    print_info(String(TCA_ADDR)); print_info("\n");
    for(uint8_t i=0; i<4; ++i) {
      tcaSelect(i);
      if (!tof.begin()) {
        print_info(F("\tFailed to boot TOF on channel "));
        print_info(String(i)); print_info("\n");
        succ_init = false;
      }
      else
      {
        print_info(F("\tSuccess booting TOF on channel "));
        print_info(String(i)); print_info("\n");
      }
    }
    print_info("OK\n");

    if(succ_init){  
      print_info("\nInitialization successful!\n");
    }
    else{
      print_info("\nInitialization failed!\n");
      while(1);
    }

    calibrateCables();

    print_info("theta, phi, L, h0, h1, h2, h3\n");
}

void loop()
{  
  coords.theta=0;
  coords.phi = 0;
  coords.length = SEGMENTS_LEN;

  for(uint8_t j=0; j<8; ++j,  coords.theta=0,coords.phi+=PI/4.0)
  {
    for(uint8_t i=0; i<8; ++i, coords.theta+=PI/28.0*1.1)
    {
      move (coords);

      // Read Sensors
        for(uint8_t i=0; i<4; ++i)
          h[i] = readSensor(i);

      readTOFs();
      coords_meas = tofs2pcc(l0, l1, l2, l3);
    
      printData();

      delay(50);
    }

    for(uint8_t i=0; i<16; ++i, coords.theta-=PI/28.0*1.1)
    {
      move (coords);

      // Read Sensors
        for(uint8_t i=0; i<4; ++i)
          h[i] = readSensor(i);

      readTOFs();
      coords_meas = tofs2pcc(l0, l1, l2, l3);
    
      printData();

      delay(50);
    }
    
    for(uint8_t i=0; i<8; ++i, coords.theta+=PI/28.0*1.1)
    {
      move (coords);

      // Read Sensors
        for(uint8_t i=0; i<4; ++i)
          h[i] = readSensor(i);

      readTOFs();
      coords_meas = tofs2pcc(l0, l1, l2, l3);
    
      printData();

      delay(50);
    }

    coords.theta = 0;
    move (coords);
  }

  coords.theta = 0;
  coords.phi = 0;
  move (coords);
  delay(10000);
}
