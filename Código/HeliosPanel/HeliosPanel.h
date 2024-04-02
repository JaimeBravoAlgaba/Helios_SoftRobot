#pragma once
  #include <Arduino.h>
  #include <Wire.h>
	#include <stdint.h>
  #include <math.h>

  #define STEPS_PER_REVOLUTION 6400
  #define ANGLE_PER_STEP 2*M_PI/(STEPS_PER_REVOLUTION*1.0)

  #define REVERSE_0 true
  #define REVERSE_1 true
  #define REVERSE_2 true
  #define REVERSE_3 true

  const byte I2C_ADDR_HS0 = 0x0A;

  // ALIAS
    static const uint8_t SEC0 = 0;
    static const uint8_t SEC1 = 1;
    static const uint8_t SEC2 = 2;
    static const uint8_t SEC_ALL = 3;
	
  // PINOUT
    // Motor Drivers
    static const uint8_t EN0 = 16; // Section 0  motor drivers enable. ERROR: pin 34 only as INPUT. Change to pin 16
    static const uint8_t EN1 = 17; // Section 1  motor drivers enable. ERROR: pin 35 only as INPUT. Change to pin 17
    static const uint8_t EN2 = 32; // Section 2  motor drivers enable.

    static const uint8_t S0  = 33; // Step pin for MX_0 motors.
    static const uint8_t S1  = 25; // Step pin for MX_1 motors.
    static const uint8_t S2  = 26; // Step pin for MX_2 motors.
    static const uint8_t S3  = 27; // Step pin for MX_3 motors.

    static const uint8_t D0  = 14; // Direction pin for MX_0 motors.
    static const uint8_t D1  = 12; // Direction pin for MX_1 motors.
    static const uint8_t D2  = 13; // Direction pin for MX_2 motors.
    static const uint8_t D3  = 15; // Direction pin for MX_3 motors.

    // I2C interface    
    static const uint8_t I2C_SDA = 21;
    static const uint8_t I2C_SCL = 22;

    // Buzzer
    static const uint8_t BUZZ = 24; // Buzzer control pin.

    // Extra GPIO interface
    static const uint8_t GPIO_0 = 0;
    static const uint8_t GPIO_1 = 4;
    static const uint8_t GPIO_2 = 16;
    static const uint8_t GPIO_3 = 17;

    // SPI interface
    static const uint8_t SPI_MOSI = 23;
    static const uint8_t SPI_MISO = 19;
    static const uint8_t SPI_SCK  = 18;
    static const uint8_t SPI_CS   = 5;

    // TIMING
    static const uint8_t READ_DELAY = 5;
    //static const float STEP_DELAY = 200.0;
    static const float STEP_DELAY = 200.0;

    // DIMENSIONS
    static const float SEGMENTS_NUM = 1.0;     // Number of segments in a section
    static const float SEGMENTS_LEN = 0.05;    // Length [m] of a segment
    static const float SEGMENTS_RC  = 0.0225;  // Radius [m] of the cable distribution circunference
    static const float SEGMENTS_RP  = 0.006;   // Radius [m] of the actuator pulley

  // Data type for sensor readings
  typedef uint16_t SensorData;

  // Data structure to manage the coordinates of a PCC section.
  struct CoordsPCC
  {
    float theta, phi;
  };

// SENSOR MANAGEMENT FUNCTIONS
void initSensor();

void updateSensor();

SensorData getSensorReading(uint8_t i);

void printSensor();

// ACTUATOR MANAGEMENT FUNCTIONS
void initActuator();

float cableIKine(CoordsPCC coords, uint8_t i);

int length2steps(float l);

void enableSection(uint8_t sec);

void disableSection(uint8_t sec);

// Execute one step in desired motors. 
void stepSection(uint8_t sec, int s0, int s1, int s2, int s3, float stepDelay);

void moveSection(uint8_t sec, CoordsPCC ref);