#include "HeliosTestBoard.h"
#include <SPI.h>

HeliosSensor hSensor(SS0);
HeliosData hd;

TaskHandle_t task_sensors;
TaskHandle_t task_loop;

ContinuumSection heliosSection(1, 0.05, 0.0225, 0.006);

void readSensors(void * pvParameters)
{
  for(;;)
  {
    hd = hSensor.update();

    for(uint8_t i=0; i<3; ++i)
    {
      Serial.print(hd.data[i]); Serial.print(",");
    }
    Serial.println(hd.data[3]);
  }
}

void mainLoop(void * pvParameters)
{
  CoordsPCC ref = {M_PI/6.0, 0};

  float l1_ini = 0.05;
  float l3_ini = 0.05;
  enableMotors();

  for(;;)
  {
    int n1 = heliosSection.length2steps(l1_ini-heliosSection.cableIKine(ref, 1));
    int n3 = heliosSection.length2steps(l3_ini-heliosSection.cableIKine(ref, 3));

    uint8_t dir1 = n1<0;
    uint8_t dir3 = n3<0;
    n1 = abs(n1);
    n3 = abs(n3);

    for(int c=0; c<n3; c++)
    {
      moveStepper('C', 1, dir3, STEP_DELAY);
    }
    for(int c=0; c<n1; c++)
    {
      moveStepper('A', 1, dir1, STEP_DELAY);
    }

    //disableMotors();
    vTaskDelete(task_sensors);
    vTaskDelete(task_loop);
  }
}

void setup()
{
  testBenchInit();
  digitalWrite(SS0, HIGH);    // disable Slave Select
  digitalWrite(SS1, HIGH);    // disable Slave Select

  Serial.begin(115200);

  int mainCore = xPortGetCoreID();

  xTaskCreatePinnedToCore(readSensors, "TaskSensors", 10000, NULL, 2, &task_sensors, !mainCore);
  delay(10);
  xTaskCreatePinnedToCore(mainLoop, "MainLoop", 10000, NULL, 2, &task_loop, mainCore);
}

void loop()
{
}