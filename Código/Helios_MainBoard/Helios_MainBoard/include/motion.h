#pragma once

#define   STP_EN  8
#define   MA_DIR  5 //Stepper shield: X_DIR
#define   MB_DIR  7 //Stepper shield: Y_DIR
#define   MC_DIR  7 //Stepper shield: Z_DIR

#define   MA_STP  2 //Stepper shield: X_STP
#define   MB_STP  3 //Stepper shield: Y_STP
#define   MC_STP  4 //Stepper shield: Z_STP

void testBenchInit();
void moveStepper(char mot, int nSteps, int dir, int stepDelay);

