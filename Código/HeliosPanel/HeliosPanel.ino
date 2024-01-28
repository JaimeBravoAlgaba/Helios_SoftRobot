// BOARD: "DOIT ESP32 DEVKIT V1"

#include "HeliosPanel.h"

TaskHandle_t task_sensors;
TaskHandle_t task_loop;

void readSensors(void * pvParameters)
{
  for(;;)
  {
    updateSensor();
    printSensor();
    delay(10);
  }
}

void mainLoop(void * pvParameters)
{
  CoordsPCC ref = {M_PI/4.0, 0};
  delay(1000);

  for(;;)
  {
    for(float phi=0.0; phi<2*M_PI; phi = phi+M_PI/100.0)
      moveSection(SEC0, CoordsPCC{M_PI/6.0, phi});
    delay(1000);
    moveSection(SEC0, CoordsPCC{0.001, 0});
  
    for(float phi=0.0; phi<2*M_PI; phi = phi+M_PI/100.0)
      moveSection(SEC1, CoordsPCC{M_PI/6.0, phi});
    delay(1000);
    moveSection(SEC1, CoordsPCC{0.001, 0});

    for(float phi=0.0; phi<2*M_PI; phi = phi+M_PI/100.0)
      moveSection(SEC2, CoordsPCC{M_PI/6.0, phi});
    delay(1000);
    moveSection(SEC2, CoordsPCC{0.001, 0});
  }
}

void setup()
{
  Serial.begin(115200);
  initSensor();
  initActuator();

  int mainCore = xPortGetCoreID();
  xTaskCreatePinnedToCore(readSensors, "TaskSensors", 10000, NULL, 2, &task_sensors, !mainCore);
  delay(10);
  xTaskCreatePinnedToCore(mainLoop, "MainLoop", 10000, NULL, 2, &task_loop, mainCore);
}

void loop(){}