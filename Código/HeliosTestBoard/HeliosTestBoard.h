#pragma once
  #include <Arduino.h>
  #include <Wire.h>
	#include <stdint.h>
  #include "motion.h"
  #include <math.h>

  #define STEPS_PER_REVOLUTION 6400
  #define ANGLE_PER_STEP 2*M_PI/(STEPS_PER_REVOLUTION*1.0)

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
  static const float STEP_DELAY = 200.0;

  #define ACTUATOR_CONFIG_A DIR_A, STP_A, DIR_B, STP_B, DIR_C, STP_C, DIR_D, STP_D, EN_MOT, STEPS_PER_REVOLUTION, 6

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
      ContinuumSection(ActuatorBench actuator, int N, float L, float rc, float rp);
      void init();
      float cableIKine(CoordsPCC coords, uint8_t i);
      int length2steps(float l);
      void move(CoordsPCC ref);

    private:
      ActuatorBench _actuator;

      int _nSegments;     // Number of segments.
      float _length;      // Segment length.
      float _rc;          // Cable disposition radius.
      float _rp;          // Pulley radius.
      float _offset[4];   // Cable offset angle.
      float _cableLengths[4];
  };