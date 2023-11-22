#pragma once
  #include <Arduino.h>
  #include <Wire.h>
	#include <stdint.h>
  #include <math.h>

  #define STEPS_PER_REVOLUTION 6400
  #define ANGLE_PER_STEP 2*M_PI/(STEPS_PER_REVOLUTION*1.0)

  #define REVERSE_A false
  #define REVERSE_B false
  #define REVERSE_C false
  #define REVERSE_D false

  const byte I2C_ADDR_HS0 = 0x0A;
	
  // PINOUT
    // Motor Drivers
    static const uint8_t EN0 = 34; // Section 0  motor drivers enable.
    static const uint8_t EN1 = 35; // Section 1  motor drivers enable.
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

  static const uint8_t READ_DELAY = 5;
  static const float STEP_DELAY = 200.0;

  #define ACTUATOR_CONFIG_0 D0, S0, D1, S1, D2, S2, D3, S3, EN0, STEPS_PER_REVOLUTION, 6

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

  // Class for managing the Helios Sensor.
  class HeliosSensor
  {
    public:
      HeliosSensor();  // Constructor
      void update();
      SensorData getReading(uint8_t i);
      void print();

    private:
      uint8_t _buffLen;
      SensorData _currReading[4] = {0,0,0,0};
  };

  // Class for managing the motors of a section
  class ActuatorBench
  {
    public:
      ActuatorBench();
      ActuatorBench(uint8_t dirA, uint8_t stpA, uint8_t dirB, uint8_t stpB, uint8_t dirC, uint8_t stpC, uint8_t dirD, uint8_t stpD, uint8_t en, int spr, float pulleyRadius);
      void init();
      void step(int sA, int sB, int sC, int sD, float stepDelay);
      void enable();
      void disable();

    private:
      uint8_t _dirA, _dirB, _dirC, _dirD; // Direction pins.
      uint8_t _stpA, _stpB, _stpC, _stpD; // Step pins.
      uint8_t _en;  // Enable pin.
      int _spr;     // Steps per revolution.
      float _pulleyRadius;
  };

  // Class for controlling the section itself
  class ContinuumSection
  {
    public:
      ContinuumSection(ActuatorBench actuator, HeliosSensor sensor, int N, float L, float rc, float rp);
      void init();
      float cableIKine(CoordsPCC coords, uint8_t i);
      int  length2steps(float l);
      
      void move(CoordsPCC ref);
      void enableMotors();
      void disableMotors();
      
      void updateSensor();
      void printSensor();

    private:
      ActuatorBench _actuator;
      HeliosSensor _sensor;

      int _nSegments;     // Number of segments.
      float _length;      // Segment length.
      float _rc;          // Cable disposition radius.
      float _rp;          // Pulley radius.
      float _offset[4];   // Cable offset angle.
      float _cableLengths[4];
  };