#include "HeliosTestBoard.h"
#include <SPI.h>

HeliosSensor hSensor;

ActuatorBench actuator(DIR_A, STP_A, DIR_B, STP_B, DIR_C, STP_C, DIR_D, STP_D, EN_MOT, STEPS_PER_REVOLUTION, 6);
ContinuumSection heliosSection(actuator, 1.0, 0.048, 0.0225, 0.006);

TaskHandle_t task_sensors;
TaskHandle_t task_loop;

void readSensors(void * pvParameters)
{
  for(;;)
  {
    hSensor.update();
    hSensor.print();
  }
}

void mainLoop(void * pvParameters)
{
  CoordsPCC ref = {M_PI/4.0, 0};
  delay(1000);
  enableMotors();

  for(;;)
  {
    for(float phi=0.0; phi<2*M_PI; phi = phi+M_PI/100.0)
      heliosSection.move(CoordsPCC{M_PI/6.0, phi});
    //disableMotors();
    //vTaskDelete(task_sensors);
    //vTaskDelete(task_loop);
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