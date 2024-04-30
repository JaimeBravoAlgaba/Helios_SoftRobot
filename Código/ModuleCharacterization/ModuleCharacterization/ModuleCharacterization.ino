#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <EEPROM.h>

#include "SparkFun_ADS122C04_ADC_Arduino_Library.h" // Modded library for compatibility with Helios project

#define HELIOS_ADDR 0x44
#define IMU_0_ADDR 0x08
#define IMU_1_ADDR 0x68

SFE_ADS122C04 heliosSensor;

float h0 = 0, h1 = 0, h2 = 0, h3 = 0; // Helios module measurement
float qx_0=0, qy_0=0, qz_0=0; // IMU_0 Euler angles
float qx_1=0, qy_1=0, qz_1=0; // IMU_1 Euler angles
float theta=0, phi=0; // Theta and phi coordinates

uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;  // Delay between samples

Adafruit_BNO055 bno_0 = Adafruit_BNO055(55, IMU_0_ADDR, &Wire);
Adafruit_BNO055 bno_1 = Adafruit_BNO055(55, IMU_1_ADDR, &Wire);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for serial port to open
  Wire.begin();

  // HELIOS INIT
  while(!heliosSensor.begin(HELIOS_ADDR)){
    Serial.println(F("Helios Module not detected. Retrying..."));
    delay(500);
  }

  heliosSensor.configureADCmode(ADS122C04_RAW_MODE);
  heliosSensor.setGain(ADS122C04_GAIN_16);

  // IMU INIT
  while(!bno_0.begin()){
    Serial.print("BNO055 not detected. Retrying...");
    delay(500);
  }

  int eeAddress = 0;
  long bnoID;

  EEPROM.get(eeAddress, bnoID);
  adafruit_bno055_offsets_t calibrationData;
  sensor_t sensor;

  /*
  *  Look for the sensor's unique ID at the beginning oF EEPROM.
  *  This isn't foolproof, but it's better than nothing.
  */
  bno_0.getSensor(&sensor);
  if (bnoID != sensor.sensor_id){
      Serial.println("\nNo Calibration Data for BNO055 with address ");
      Serial.println(IMU_0_ADDR, HEX);
      delay(500);
  }
  else
  {
      eeAddress += sizeof(long);
      EEPROM.get(eeAddress, calibrationData);
      bno_0.setSensorOffsets(calibrationData);
      Serial.print("\n\nCalibration data loaded into BNO055 with address ");
      Serial.println(IMU_0_ADDR, HEX);
  }

  // IMU INIT
  while(!bno_1.begin()){
    Serial.print("BNO055 not detected. Retrying...");
    delay(500);
  }
    
  eeAddress = 0;
  bno_1.getSensor(&sensor);
  if (bnoID != sensor.sensor_id){
      Serial.println("\nNo Calibration Data for BNO055 with address ");
      Serial.println(IMU_1_ADDR, HEX);
      delay(500);
  }
  else
  {
      eeAddress += sizeof(long);
      EEPROM.get(eeAddress, calibrationData);
      bno_1.setSensorOffsets(calibrationData);
      Serial.print("\n\nCalibration data loaded into BNO055 with address ");
      Serial.println(IMU_1_ADDR, HEX);
  }

  Serial.println("Theta, Phi, h0, h1, h2, h3");
}

void loop() {
  // ORIENTATION MEASUREMENT
    sensors_event_t orientationData;
    bno_0.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    qx_0 = orientationData.orientation.x;
    qy_0 = orientationData.orientation.y;
    qz_0 = orientationData.orientation.z;

    bno_1.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    qx_1 = orientationData.orientation.x;
    qy_1 = orientationData.orientation.y;
    qz_1 = orientationData.orientation.z;

  // HELIOS MEASUREMENT
    heliosSensor.setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS);
    h0 = heliosSensor.readRawVoltage() / 4096000.0;

    heliosSensor.setInputMultiplexer(ADS122C04_MUX_AIN1_AVSS);
    h1 = heliosSensor.readRawVoltage() / 4096000.0;

    heliosSensor.setInputMultiplexer(ADS122C04_MUX_AIN2_AVSS);
    h2 = heliosSensor.readRawVoltage() / 4096000.0;

    heliosSensor.setInputMultiplexer(ADS122C04_MUX_AIN3_AVSS);
    h3 = heliosSensor.readRawVoltage() / 4096000.0;

  // CALCULATIONS
    phi = atan2(qz_1,-qy_1);
    theta = (abs(qy_1/cos(phi)) + abs(qz_1/sin(phi)))/2.0;

    float phi_aux = atan2(qz_0,-qy_0);
    theta = theta - (abs(qy_0/cos(phi_aux)) + abs(qz_0/sin(phi_aux)))/2.0;
    phi = phi - phi_aux;

    phi = phi*180/PI + (qx_1-qx_0) - 45;
    while(phi>=360){
      phi = phi-360; //Correción de rango positivo
    }    
    while(phi<0){
      phi = phi+360; //Correción de rango negativo
    }

  // PRINT DATA
    Serial.print(theta*PI/180.0, 6);  Serial.print(",");
    Serial.print(phi*PI/180.0, 6);    Serial.print(",");
    Serial.print(h0, 6);     Serial.print(",");
    Serial.print(h1, 6);     Serial.print(",");
    Serial.print(h2, 6);     Serial.print(",");
    Serial.println(h3, 6);
}