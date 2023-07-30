#include "HeliosTestBoard.h"
#include <SPI.h>

HeliosSensor hSensor;

ActuatorBench actuator(DIR_A, STP_A, DIR_B, STP_B, DIR_C, STP_C, DIR_D, STP_D, EN_MOT, STEPS_PER_REVOLUTION, 6);

TaskHandle_t task_sensors;
TaskHandle_t task_loop;

ContinuumSection heliosSection(1, 0.05, 0.0225, 0.006);

void readSensors(void * pvParameters)
{
  for(;;)
  {
    hSensor.update();
    hSensor.print();
    delay(50);
  }
}

void mainLoop(void * pvParameters)
{
  CoordsPCC ref = {M_PI/6.0, 3*M_PI/2.0};
  float l1_ini = 0.05;
  float l2_ini = 0.05;
  float l3_ini = 0.05;
  float l4_ini = 0.05;
  delay(1000);
  enableMotors();

  for(;;)
  {
    int n1 = heliosSection.length2steps(l1_ini-heliosSection.cableIKine(ref, 0));
    int n2 = heliosSection.length2steps(l2_ini-heliosSection.cableIKine(ref, 1));
    int n3 = heliosSection.length2steps(l3_ini-heliosSection.cableIKine(ref, 2));
    int n4 = heliosSection.length2steps(l4_ini-heliosSection.cableIKine(ref, 3));

    int N = max({abs(n1), abs(n2), abs(n3), abs(n4)});
    float T = N*STEP_DELAY; // Time to execute the profile [us]

    float v[4] = {n1/T, n2/T, n3/T, n4/T};
    float r[4] = {0, 0, 0, 0};
    int   s[4] = {0, 0, 0, 0};

    for (int i=0; i<N; ++i)
    {
      for(uint8_t i=0; i<4; ++i)
      {
        r[i] = r[i] + v[i]*STEP_DELAY;
        s[i] = floor(r[i]);
        r[i] = r[i] - float(s[i]);
      }
      actuator.step(s[0], s[1], s[2], s[3], STEP_DELAY);
    }

    disableMotors();
    //vTaskDelete(task_sensors);
    vTaskDelete(task_loop);
  }
}

void setup()
{
  Serial.begin(115200);
  testBenchInit();

  int mainCore = xPortGetCoreID();

  xTaskCreatePinnedToCore(readSensors, "TaskSensors", 10000, NULL, 2, &task_sensors, !mainCore);
  delay(10);
  xTaskCreatePinnedToCore(mainLoop, "MainLoop", 10000, NULL, 2, &task_loop, mainCore);
}

void loop()
{
}