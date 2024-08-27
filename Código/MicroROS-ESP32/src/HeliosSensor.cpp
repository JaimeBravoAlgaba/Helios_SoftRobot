#include "HeliosPanel.h"

SFE_ADS122C04 sensor[N_MODULES];

float sensorData[N_MODULES*N_SENSORS] = {0};      // Sensor data measured for "N" modules: [h00, h01, h02, h03, h10, h11, ... hN0, hN1, hN2, hN3]
uint32_t sensorData_ini[N_MODULES*N_SENSORS] = {0};  // Initial sensor data measured for "N" modules

void initSensor()
{
  Wire.begin();
  Wire.setClock(I2C_FREQ);

  for (int i = 0; i < N_MODULES; i++) {
    sensor[i].begin(I2C_SENSOR_ADDR[i]);

    sensor[i].configureADCmode(ADS122C04_RAW_MODE); // Configure the PT100 for 3-wire mode
    sensor[i].setGain(ADS122C04_GAIN_128);
    sensor[i].setConversionMode(ADS122C04_CONVERSION_MODE_CONTINUOUS);
    sensor[i].start();
  }

  for (int i = 0; i < N_MODULES; i++) {
    for (int j = 0; j < N_SENSORS; j++) {
      updateSensorDataIni(i,j);
    }
  }
}
uint32_t updateSensorDataIni(uint8_t module_index, uint8_t sensor_index)
{
  sensor[module_index].setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS + sensor_index);
  return sensorData_ini[N_SENSORS*module_index+sensor_index] = (int32_t)sensor[module_index].readRawVoltage();
}

float updateSensorData(uint8_t module_index, uint8_t sensor_index)
{
  sensor[module_index].setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS + sensor_index);
  int aux = sensor[module_index].readRawVoltage() - sensorData_ini[N_SENSORS*module_index+sensor_index];
  return sensorData[N_SENSORS*module_index+sensor_index] = aux / (float)sensorData_ini[N_SENSORS*module_index+sensor_index];
}

void updateModuleData()
{
  for (int i = 0; i < N_MODULES; i++) {
    for (int j = 0; j < N_SENSORS; j++) {
      updateSensorData(i, j);
    }
  }
}