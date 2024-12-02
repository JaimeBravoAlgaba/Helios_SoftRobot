#include <Adafruit_VL53L0X.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <EEPROM.h>
#include "CNC_Shield_UNO.h"

#include <SparkFun_ADS122C04_ADC_Arduino_Library.h> // Modded library for compatibility with Helios project


#define HELIOS_ADDR 0x4A
#define IMU_0_ADDR 0x08
#define IMU_1_ADDR 0x68
#define TCAADDR 0x70

SFE_ADS122C04 heliosSensor;
Adafruit_BNO055 bno_0 = Adafruit_BNO055(55, IMU_0_ADDR, &Wire);
Adafruit_VL53L0X tof = Adafruit_VL53L0X();

uint32_t h[4] = {0, 0, 0, 0}; // Helios module measurement 
float cableLengths[4] = {SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN};
sensors_event_t orientationData;

CoordsPCC coords = {0, 0};
CoordsPCC coordsIMU = {0,0};
float qx_0 = 0, qy_0 = 0, qz_0 = 0;
float l0 = 0, l1 = 0, l2 = 0, l3 = 0;

CoordsPCC euler2pcc(float qx, float qy, float qz)
{
  CoordsPCC coords;
  coords.phi = atan2(qz, qy);
  coords.theta = qy*cos(qx)+qz*sin(qx);
  return coords;
}

uint32_t readSensor(uint8_t i)
{
      heliosSensor.setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS + i);
      delay(50);
      return heliosSensor.readADC(); 
}

void readTOFs()
{
      tcaSelect(0);
      
}

void tcaSelect(uint8_t i)
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
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
    //Serial.print(h[i]); Serial.print(",");
    int thr = 0.007*h[i];
    uint32_t u = h[i];
    dn[i] = n;
    
    while(abs((int)h[i] - (int)u) < thr)
    {
      stepParallel(dn);
      u = readSensor(i);
      //Serial.print(u); Serial.print(",");
    }
    cableLengths[i] = SEGMENTS_LEN;
    dn[i] = 0;
    //Serial.print("Motor "); Serial.print(i); Serial.println(" ready!");
    delay(100);
  }
}

void printData()
{

      Serial.print(qx_0); Serial.print(",");
      Serial.print(qy_0); Serial.print(",");
      Serial.print(qz_0); Serial.print(",");

      Serial.print(coords.theta*180/PI, 4); Serial.print(",");
      Serial.print(coords.phi*180/PI, 4);   Serial.print(",");

      Serial.print(coordsIMU.theta, 4); Serial.print(",");
      Serial.print(coordsIMU.phi, 4);   Serial.print(",");

      Serial.print(h[0]); Serial.print(",");
      Serial.print(h[1]); Serial.print(",");
      Serial.print(h[2]); Serial.print(",");
      Serial.println(h[3]);  
}

void setup()
{
  setupCNC();

  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for serial port to open
  Wire.begin();

  // HELIOS INIT
    while(!heliosSensor.begin(HELIOS_ADDR)){
      Serial.println(F("Helios Module not detected. Retrying..."));
      delay(500);
    }

    heliosSensor.configureADCmode(ADS122C04_RAW_MODE);
    heliosSensor.setGain(ADS122C04_GAIN_128);
    heliosSensor.setConversionMode(ADS122C04_CONVERSION_MODE_CONTINUOUS);
    heliosSensor.start();

    Serial.print("Helios Module"); Serial.print(HELIOS_ADDR, HEX); Serial.println(" initialized.");

  // BNO055 INIT
    while(!bno_0.begin())
    {
      Serial.println("BNO055 not detected. Retrying...");
      delay(500);
    }

    int eeAddress = 0;
    long bnoID;

    EEPROM.get(eeAddress, bnoID);
    adafruit_bno055_offsets_t calibrationData;
    sensor_t sensor;

    bno_0.getSensor(&sensor);
    if(bnoID != sensor.sensor_id)
    {
      Serial.println("\nNo calibration data for BNO055 with address ");
      Serial.println(IMU_0_ADDR, HEX);
      delay(500);
    }
    else
    {
      eeAddress += sizeof(long);
      EEPROM.get(eeAddress, calibrationData);
      bno_0.setSensorOffsets(calibrationData);
      Serial.print("\nCalibration data loaded into BNO055 with address ");
      Serial.println(IMU_0_ADDR);
    }


    // TCA9548A SCAN  
    Serial.println("Adafruit VL53L0X test");
    for(uint8_t i=0; i<4; ++i) {
      tcaSelect(i);
      if (!tof.begin()) {
        Serial.print(F("Failed to boot VL53L0X on channel "));
        Serial.println(i);
      }
    }
  
    Serial.println("End");

    calibrateCables();

    Serial.println("eul_x, eul_y, eul_z, theta_ref, phi_ref, theta, phi, h0, h1, h2, h3");
}

void loop()
{
  /*
  coords.theta=0;
  coords.phi = 0;

  for(uint8_t j=0; j<24; ++j,  coords.theta=0,coords.phi+=PI/12.0)
  {
    for(uint8_t i=0; i<8; ++i, coords.theta+=PI/28.0*1.1)
    {
      move (coords);

      // Read IMU
        sensors_event_t orientationData;
        bno_0.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
        qx_0 = orientationData.orientation.x;
        qy_0 = orientationData.orientation.y;
        qz_0 = orientationData.orientation.z;

        coordsIMU.theta = sqrt(qy_0*qy_0 + qz_0*qz_0);
        coordsIMU.phi = qx_0-atan2(qy_0, qz_0)*180/PI+45;

        coordsIMU.phi = (coordsIMU.phi<0)? coordsIMU.phi+360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.phi>360)? coordsIMU.phi-360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.theta<0)? coordsIMU.phi+180:coordsIMU.phi;
        coordsIMU.theta = abs(coordsIMU.theta);
      
      // Read Helios
        for(uint8_t i=0; i<4; ++i)
          h[i] = readSensor(i);

      Serial.print(qx_0); Serial.print(",");
      Serial.print(qy_0); Serial.print(",");
      Serial.print(qz_0); Serial.print(",");

      Serial.print(coords.theta*180/PI, 4); Serial.print(",");
      Serial.print(coords.phi*180/PI, 4);   Serial.print(",");

      Serial.print(coordsIMU.theta, 4); Serial.print(",");
      Serial.print(coordsIMU.phi, 4);   Serial.print(",");

      Serial.print(h[0]); Serial.print(",");
      Serial.print(h[1]); Serial.print(",");
      Serial.print(h[2]); Serial.print(",");
      Serial.println(h[3]);    

      delay(50);
    }
    coords.theta = 0;
    move (coords);
    calibrateCables();
  }
  */
  
  coords.theta=0;
  coords.phi = 0;

  for(uint8_t j=0; j<8; ++j,  coords.theta=0,coords.phi+=PI/4.0)
  {
    for(uint8_t i=0; i<8; ++i, coords.theta+=PI/28.0*1.1)
    {
      move (coords);
      
      // Read Helios
        for(uint8_t i=0; i<4; ++i)
          h[i] = readSensor(i);

      // Read IMU
        bno_0.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
        qx_0 = orientationData.orientation.x;
        qy_0 = orientationData.orientation.y;
        qz_0 = orientationData.orientation.z;

        coordsIMU.theta = sqrt(qy_0*qy_0 + qz_0*qz_0);
        coordsIMU.phi = qx_0-atan2(qy_0, qz_0)*180/PI+45;

        coordsIMU.phi = (coordsIMU.phi<0)? coordsIMU.phi+360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.phi>360)? coordsIMU.phi-360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.theta<0)? coordsIMU.phi+180:coordsIMU.phi;
        coordsIMU.theta = abs(coordsIMU.theta);
      
      // Print Data
      printData();

      delay(50);
    }

    for(uint8_t i=0; i<16; ++i, coords.theta-=PI/28.0*1.1)
    {
      move (coords);
      
      // Read Helios
        for(uint8_t i=0; i<4; ++i)
          h[i] = readSensor(i);

      // Read IMU
        bno_0.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
        qx_0 = orientationData.orientation.x;
        qy_0 = orientationData.orientation.y;
        qz_0 = orientationData.orientation.z;

        coordsIMU.theta = sqrt(qy_0*qy_0 + qz_0*qz_0);
        coordsIMU.phi = qx_0-atan2(qy_0, qz_0)*180/PI+45;

        coordsIMU.phi = (coordsIMU.phi<0)? coordsIMU.phi+360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.phi>360)? coordsIMU.phi-360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.theta<0)? coordsIMU.phi+180:coordsIMU.phi;
        coordsIMU.theta = abs(coordsIMU.theta);
      
      // Print Data
      printData();

      delay(50);
    }
    
    for(uint8_t i=0; i<8; ++i, coords.theta+=PI/28.0*1.1)
    {
      move (coords);
      
      // Read Helios
        for(uint8_t i=0; i<4; ++i)
          h[i] = readSensor(i);

      // Read IMU
        bno_0.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
        qx_0 = orientationData.orientation.x;
        qy_0 = orientationData.orientation.y;
        qz_0 = orientationData.orientation.z;

        coordsIMU.theta = sqrt(qy_0*qy_0 + qz_0*qz_0);
        coordsIMU.phi = qx_0-atan2(qy_0, qz_0)*180/PI+45;

        coordsIMU.phi = (coordsIMU.phi<0)? coordsIMU.phi+360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.phi>360)? coordsIMU.phi-360:coordsIMU.phi;
        coordsIMU.phi = (coordsIMU.theta<0)? coordsIMU.phi+180:coordsIMU.phi;
        coordsIMU.theta = abs(coordsIMU.theta);
      
      // Print Data
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