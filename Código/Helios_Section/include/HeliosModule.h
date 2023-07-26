#pragma once
#include <Arduino.h>

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

// Protocol codes
const uint8_t SPI_REQ_A  = lowByte(0x1);
const uint8_t SPI_REQ_B  = lowByte(0x3);
const uint8_t SPI_REQ_C  = lowByte(0x5);
const uint8_t SPI_REQ_D  = lowByte(0x7);
const uint8_t SPI_OK     = lowByte(0xE);
const uint8_t SPI_ERR    = lowByte(0xF); 


// Communication Functions
void initCommunications();

// Sensing Functions
void heliosInit();
void readSensors(SensorData*);