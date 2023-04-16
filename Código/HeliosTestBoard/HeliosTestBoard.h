#pragma once
	#include <stdint.h>
  #include "motion.h"
  #include "kinematics.h"
	
	static const uint8_t DIR_A  = 32;
	static const uint8_t STP_A  = 33;
	static const uint8_t DIR_B  = 25;
	static const uint8_t STP_B  = 26;
	static const uint8_t DIR_C  = 27;
	static const uint8_t STP_C  = 14;
	static const uint8_t DIR_D  = 12;
	static const uint8_t STP_D  = 13;
	//static const uint8_t MOSI   = 23;
	//static const uint8_t MISO   = 19;
	//static const uint8_t SCK    = 18;
	static const uint8_t SS1    =  5;
	static const uint8_t SS0    =  4;
	static const uint8_t EN_MOT =  0;
	static const uint8_t LED_IND = 6;

  static const uint8_t READ_DELAY = 5;
  static const uint8_t STEP_DELAY = 1000;

  union HeliosData
  {
    struct structData
    {
      uint16_t p0, p1, p2, p3;
    } data;
    
    char data_bytes[sizeof(data)];
  };