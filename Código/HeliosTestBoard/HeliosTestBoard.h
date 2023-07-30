#pragma once
  #include <Arduino.h>
  #include <Wire.h>
	#include <stdint.h>
  #include "motion.h"
  #include <math.h>

  #define STEPS_PER_REVOLUTION 7000
  #define ANGLE_PER_STEP 2*M_PI/(STEPS_PER_REVOLUTION*1.0)
  #define V_MAX 3000.0 // Steps per second

  #define REVERSE_A false
  #define REVERSE_B false
  #define REVERSE_C false
  #define REVERSE_D false

  const byte I2C_ADDR_HS0 = 0x0A;
	
	static const uint8_t DIR_A  = 32;
	static const uint8_t STP_A  = 33;
	static const uint8_t DIR_B  = 25;
	static const uint8_t STP_B  = 26;
	static const uint8_t DIR_C  = 27;
	static const uint8_t STP_C  = 14;
	static const uint8_t DIR_D  = 12;
	static const uint8_t STP_D  = 13;
	static const uint8_t EN_MOT =  0;
	static const uint8_t LED_IND = 6;

  static const uint8_t READ_DELAY = 5;
  static const float STEP_DELAY = 500.0;

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
      ActuatorBench(uint8_t dirA, uint8_t stpA, uint8_t dirB, uint8_t stpB, uint8_t dirC, uint8_t stpC, uint8_t dirD, uint8_t stpD, uint8_t en, int spr, float pulleyRadius);
      void step(int sA, int sB, int sC, int sD, float stepDelay);

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
      ContinuumSection(int N, float L, float rc, float rp);

      float cableIKine(CoordsPCC coords, uint8_t i);
      int length2steps(float l);

    private:
      int _nSegments;     // Number of segments.
      float _length;      // Segment length.
      float _rc;          // Cable disposition radius.
      float _rp;          // Pulley radius.
      float _offset[4];   // Cable offset angle.
  };