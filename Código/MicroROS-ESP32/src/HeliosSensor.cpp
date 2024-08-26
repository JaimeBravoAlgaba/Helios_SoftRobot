#include "HeliosPanel.h"

SFE_ADS122C04 sensor[N_MODULES];

float sensorData[N_MODULES*N_SENSORS] = {0};      // Sensor data measured for "N" modules: [h00, h01, h02, h03, h10, h11, ... hN0, hN1, hN2, hN3]
float sensorData_ini[N_MODULES*N_SENSORS] = {0};  // Initial sensor data measured for "N" modules

void initSensor()
{
  Wire.begin();

  for (int i = 0; i < N_MODULES; i++) {
    sensor[i].begin(I2C_SENSOR_ADDR[i]);

    sensor[i].configureADCmode(ADS122C04_RAW_MODE); // Configure the PT100 for 3-wire mode
    sensor[i].setGain(ADS122C04_GAIN_16);
  }

  updateSensors();
  for (int i = 0; i < N_MODULES*4; i++) {
    sensorData_ini[i] = sensorData[i];
  }
}

void updateSensors()
{
  for (int i = 0; i < N_MODULES; i++) {
    sensor[i].setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS);
    sensorData[4*i] = (sensor[i].readADC() - sensorData_ini[4*i]) / sensorData_ini[4*i];

    sensor[i].setInputMultiplexer(ADS122C04_MUX_AIN1_AVSS);
    sensorData[4*i+1] = sensor[i].readADC() - sensorData_ini[4*i+1] / sensorData_ini[4*i+1];

    sensor[i].setInputMultiplexer(ADS122C04_MUX_AIN2_AVSS);
    sensorData[4*i+2] = sensor[i].readADC() - sensorData_ini[4*i+2] / sensorData_ini[4*i+2];

    sensor[i].setInputMultiplexer(ADS122C04_MUX_AIN3_AVSS);
    sensorData[4*i+3] = sensor[i].readADC() - sensorData_ini[4*i+3] / sensorData_ini[4*i+3];
  }
}