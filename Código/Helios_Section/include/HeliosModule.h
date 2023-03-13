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
#define LED_ON_TIME 50 //ms

// Data structure
struct HeliosData
{
    uint16_t p0;
    uint16_t p1;
    uint16_t p2;
    uint16_t p3;
};

union HeliosData_union
{
   HeliosData data;   
   char data_bytes[sizeof(data)];
};

// Communication Functions
void initCommunications();
boolean commandAvailable();
byte readCommand();

// Sensing Functions
void heliosInit();
HeliosData readSensors();