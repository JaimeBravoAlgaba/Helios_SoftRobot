#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <SoftWire.h>
#include <AsyncDelay.h>

// I2C Communications
const byte I2C_ADDR = 0x0A; // I2C Address of the board
const byte IMU_ADDR = 0x68; // SW I2C Address of the IMU (MPU6050 module)
const uint8_t SW_SDA = 14;  // SW I2C SDA pin
const uint8_t SW_SCL = 15;  // SW I2C SCL pin

// Photodiodes Sensing Pins:
#define P0 A0
#define P1 A1
#define P2 A2
#define P3 A3
#define P4 A4
#define P5 A5
#define P6 A6
#define P7 A7

// LED Control Pin
#define LED 9
#define LED_ON_TIME 100 //ms

// Data type for sensor readings
typedef uint16_t SensorData;

// Data structure for IMU readings
struct ImuData
{
    int16_t acc_x, acc_y, acc_z;    // variables for accelerometer raw data
    int16_t gyr_x, gyr_y, gyr_z;    // variables for gyro raw data
    int16_t temp;                   // variable for temperature data
};

// Periods for moving average filter
#define MVA_PERIODS 5.0

// Communication Functions
void initCommunications();
void requestEvent();

// Sensing Functions
void heliosInit();
void readSensors(SensorData*);

// IMU functions
char* convert_int16_to_str(int16_t i);
void initIMU();
void readIMU(ImuData* imu_data);
void printIMU(ImuData* imu_data);