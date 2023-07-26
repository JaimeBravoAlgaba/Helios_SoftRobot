#pragma once
#include <Arduino.h>
#include <Wire.h>

// I2C Address
const byte I2C_ADDR = 0x0A;

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

// Periods for moving average filter
#define MVA_PERIODS 5.0

// Communication Functions
void initCommunications();
void requestEvent();

// Sensing Functions
void heliosInit();
void readSensors(SensorData*);