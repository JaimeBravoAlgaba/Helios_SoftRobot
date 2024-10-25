#pragma once
  #include <Arduino.h>
  #include <Wire.h>
  #include "ADS122C04_ADC.h"
	#include <stdint.h>
  #include <math.h>

  #define STEPS_PER_REVOLUTION 6400 // Steps per revolution of the motor
  #define GEAR_RATIO 1/20.0         // Gear ratio of the motor (geared pulley)
  #define ANGLE_PER_STEP 2*M_PI/(STEPS_PER_REVOLUTION*1.0)*GEAR_RATIO // Angle per step of the motor, in radians

  #define REVERSE_0 false // Reverse direction of motor 0
  #define REVERSE_1 false // Reverse direction of motor 1
  #define REVERSE_2 false // Reverse direction of motor 2
  #define REVERSE_3 false // Reverse direction of motor 3

  // CONSTRUCTION PARAMETERS
    static const uint8_t N_SECTIONS = 3;   // Number of sections    
    static const uint8_t N_CABLES = 4;     // Number of cables in each section
    static const uint8_t N_SEGMENTS = 2;   // Number of segments in each section
    static const uint8_t N_MODULES= 6;    // Number of Helios modules in the robot
    static const uint8_t N_SENSORS = 4;    // Number of sensors in each Helios module

    static const float SEGMENTS_LEN = 0.05;    // Length [m] of a segment
    static const float SEGMENTS_RC  = 0.0225;  // Radius [m] of the cable distribution circunference
    static const float SEGMENTS_RP  = 0.006;   // Radius [m] of the actuator pulley

    static const float CABLE_MOUNT_OFFSET = 2.0*M_PI/float(N_CABLES); // Offset between cables in a section in phi coordinate [rad]
    static const float SECTION_MOUNT_OFFSET = 6*M_PI/180.0; // Offset between sections, in phi coordinate [rad]

    #define MOUNT_MATRIX(delta) {{cos(delta), sin(delta)}, {cos(delta+CABLE_MOUNT_OFFSET), sin(delta+CABLE_MOUNT_OFFSET)}, {cos(delta+2*CABLE_MOUNT_OFFSET), sin(delta+2*CABLE_MOUNT_OFFSET)}, {cos(delta+3*CABLE_MOUNT_OFFSET), sin(delta+3*CABLE_MOUNT_OFFSET)}};

    static const float mountMatrix_0[N_CABLES][2] = MOUNT_MATRIX(SECTION_MOUNT_OFFSET);  // Mount matrix for section 0
    static const float mountMatrix_1[N_CABLES][2] = MOUNT_MATRIX(-SECTION_MOUNT_OFFSET); // Mount matrix for section 1
    static const float mountMatrix_2[N_CABLES][2] = MOUNT_MATRIX(0);                     // Mount matrix for section 2

  // SECTION MANAGEMENT
    static const uint8_t SEC0 = 0;     // Select Section 0
    static const uint8_t SEC1 = 1;     // Select Section 1
    static const uint8_t SEC2 = 2;     // Select Section 2
    static const uint8_t SEC_ALL = 3;  // Select all sections
	
  // PINOUT
    // Motor Drivers
    static const uint8_t EN0 = 16; // Section 0  motor drivers enable. ERROR: pin 34 only as INPUT. Change to pin 16
    static const uint8_t EN1 = 17; // Section 1  motor drivers enable. ERROR: pin 35 only as INPUT. Change to pin 17
    static const uint8_t EN2 = 32; // Section 2  motor drivers enable.
    static const uint8_t ENABLE_PINS[N_SECTIONS] = {EN0, EN1, EN2}; // Array with the enable pins for each section

    static const uint8_t S0  = 33; // Step pin for MX_0 motors.
    static const uint8_t S1  = 26; // Step pin for MX_1 motors.
    static const uint8_t S2  = 25; // Step pin for MX_2 motors.
    static const uint8_t S3  = 27; // Step pin for MX_3 motors.
    static const uint8_t STEP_PINS[N_CABLES] = {S0, S1, S2, S3}; // Array with the step pins for each smotor

    static const uint8_t D0  = 14; // Direction pin for MX_0 motors.
    static const uint8_t D1  = 12; // Direction pin for MX_1 motors.
    static const uint8_t D2  = 13; // Direction pin for MX_2 motors.
    static const uint8_t D3  = 15; // Direction pin for MX_3 motors.
    static const uint8_t DIR_PINS[N_CABLES] = {D0, D1, D2, D3}; // Array with the direction pins for each motor

    // I2C interface
    const uint32_t I2C_FREQ = 400000; // I2C clock frequency: 100kHz (standard mode) or 400kHz (fast mode)
    const byte I2C_TOOL_ADDR = 0x70; // I2C address for the tool module
    const byte I2C_SENSOR_ADDR[N_MODULES] = {0x40, 0x41, 0x44, 0x45, 0x48, 0x4A}; // I2C addresses for Helios Modules
    static const uint8_t I2C_SDA = 21;
    static const uint8_t I2C_SCL = 22;

    // Buzzer
    static const uint8_t BUZZ = 2; // Buzzer control pin.

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
    static const uint8_t READ_DELAY = 5;  // Delay applied to the sensor readings [us]

    static const float STEP_DELAY = 70.0; // Pulse period for the driver step signal [us]
    static const float STEP_RATIO = 0.8;  // Percentage of STEP_DELAY with step in HIGH

  // Data type for sensor readings
  typedef uint16_t SensorData;

  // Data structure to manage the coordinates of a PCC section.
  struct CoordsPCC
  {
    float theta, phi;
  };

// UTILITIES

void printCoordsPCC(CoordsPCC c);

void buzz(uint8_t n, uint8_t tone, int t_high, int t_low);

bool i2cCheckDevice(byte address);

// SENSOR MANAGEMENT FUNCTIONS
void initSensor();

uint32_t updateSensorDataIni(uint8_t module_index, uint8_t sensor_index);

float updateSensorData(uint8_t module_index, uint8_t sensor_index);

void updateModuleData();

// ACTUATOR MANAGEMENT FUNCTIONS
void initActuator();

void resetCableLengths();

float cableIKine(CoordsPCC coords, uint8_t sec, uint8_t cable);

int length2steps(float l);

void enableSection(uint8_t sec);

void disableSection(uint8_t sec);

// Execute one step in desired motors. 
void stepSection(uint8_t sec, const int s[N_CABLES], float stepDelay);

void moveSection(uint8_t sec, const float l_ref[N_CABLES]);

void moveRobot(const float ref[N_SECTIONS*N_CABLES]);