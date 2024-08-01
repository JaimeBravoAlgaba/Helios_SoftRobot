#include "HeliosPanel.h"

SFE_ADS122C04 sensor[N_SENSORS];

float sensorData[N_SENSORS*2] = {0};

void initSensor()
{
  Wire.begin();

  for (int i = 0; i < N_SENSORS; i++) {
    sensor[i].begin(I2C_SENSOR_ADDR[i]);

    sensor[i].configureADCmode(ADS122C04_RAW_MODE); // Configure the PT100 for 3-wire mode
    sensor[i].setGain(ADS122C04_GAIN_16);
  }
}

void updateSensors()
{
  for (int i = 0; i < N_SENSORS; i++) {
    sensor[i].setInputMultiplexer(ADS122C04_MUX_AIN0_AIN2);
    sensorData[2*i] = sensor[i].readRawVoltage() / 4096000.0;

    sensor[i].setInputMultiplexer(ADS122C04_MUX_AIN1_AIN3);
    sensorData[2*i+1] = sensor[i].readRawVoltage() / 4096000.0;
  }
}