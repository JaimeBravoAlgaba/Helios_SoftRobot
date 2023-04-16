#pragma once
  // Pins initialization.
  void testBenchInit();
  
  void moveStepper(char mot, int nSteps, int dir, int stepDelay);
  void disableMotors();
  void enableMotors ();