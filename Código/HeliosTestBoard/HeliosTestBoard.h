#pragma once
  #include <Arduino.h>
  #include <SPI.h>
	#include <stdint.h>
  #include "motion.h"
  #include <math.h>

  #define STEPS_PER_REVOLUTION 7000
  #define ANGLE_PER_STEP 2*M_PI/(STEPS_PER_REVOLUTION*1.0)
	
	static const uint8_t DIR_A  = 32;
	static const uint8_t STP_A  = 33;
	static const uint8_t DIR_B  = 25;
	static const uint8_t STP_B  = 26;
	static const uint8_t DIR_C  = 27;
	static const uint8_t STP_C  = 14;
	static const uint8_t DIR_D  = 12;
	static const uint8_t STP_D  = 13;
	static const uint8_t SS1    =  5;
	static const uint8_t SS0    =  4;
	static const uint8_t EN_MOT =  0;
	static const uint8_t LED_IND = 6;

  static const uint8_t READ_DELAY = 5;
  static const uint8_t STEP_DELAY = 5000;

  // Data structure for sensor readings.
  union HeliosData
  {
    uint16_t data[4] = {0, 0, 0, 0};    
    char data_bytes[sizeof(data)];
  };

  // Data structure to manage the coordinates of a PCC section.
  struct CoordsPCC
  {
    float theta, phi;
  };

  // Class for managing the Helios Sensor.
  class HeliosSensor : SPIClass
  {
    public:
      HeliosSensor(uint8_t ssPin);  // Constructor
      HeliosData update();
      uint16_t getReading(uint8_t i);
      void print();

    private:
      uint8_t _ssPin;
      uint8_t _buffLen;
      HeliosData _currReading;
  };

  class Actuator
  {
    public:
      Actuator(uint8_t dir, uint8_t stp, uint8_t en, int spr, float pulleyRadius);
      void step(int steps, uint32_t stepDelay);

    private:
      uint8_t _dir; // Direction pin.
      uint8_t _stp; // Step pin.
      uint8_t _en;  // Enable pin.
      int _spr;     // Steps per revolution.
      float _pulleyRadius;
      bool _reverse;
  };

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