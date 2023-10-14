#include "HeliosTestBoard.h"

ContinuumSection heliosSection(ActuatorBench(ACTUATOR_CONFIG_A), HeliosSensor(), SEGMENTS_NUM, SEGMENTS_LEN, SEGMENTS_RC, SEGMENTS_RP);

TaskHandle_t task_sensors;
TaskHandle_t task_loop;

void readSensors(void * pvParameters)
{
  for(;;)
  {
    heliosSection.updateSensor();
    heliosSection.printSensor();
    delay(10);
  }
}

void mainLoop(void * pvParameters)
{
  CoordsPCC ref = {M_PI/4.0, 0};
  delay(1000);
  heliosSection.enableMotors();

  for(;;)
  {
    for(float phi=0.0; phi<2*M_PI; phi = phi+M_PI/100.0)
      heliosSection.move(CoordsPCC{M_PI/6.0, phi});
    delay(1000);
    heliosSection.move(CoordsPCC{0.001, 0});
  }
}

void setup()
{
  Serial.begin(115200);
  heliosSection.init();

  int mainCore = xPortGetCoreID();
  xTaskCreatePinnedToCore(readSensors, "TaskSensors", 10000, NULL, 2, &task_sensors, !mainCore);
  delay(10);
  xTaskCreatePinnedToCore(mainLoop, "MainLoop", 10000, NULL, 2, &task_loop, mainCore);
}

void loop(){}