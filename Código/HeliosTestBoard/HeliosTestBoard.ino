#include "HeliosTestBoard.h"
#include <SPI.h>
SPIClass spi_sensor;

TaskHandle_t task_sensors;
TaskHandle_t task_loop;

HeliosData hd;
const uint8_t buffLen = sizeof(hd);

Section heliosSection(1, 0.05, 0.0225, 0.006);

void readSensors(void * pvParameters)
{
  for(;;)
  {
    for (uint8_t j=0; j<buffLen; j++)
    {
        digitalWrite(SS0, LOW); // enable Slave Select
        delay(READ_DELAY);
        hd.data_bytes[j] = (byte)spi_sensor.transfer (0x01);
        digitalWrite(SS0, HIGH); // disable Slave Select
    }
    int p0 = hd.data.p0;
    int p1 = hd.data.p1;
    int p2 = hd.data.p2;
    int p3 = hd.data.p3;

    digitalWrite(SS1, LOW);

    Serial.print(0); Serial.print(",");
    Serial.print(p0); Serial.print(",");
    Serial.print(p1); Serial.print(",");
    Serial.print(p2); Serial.print(",");
    Serial.println(p3);
  }
}

void mainLoop(void * pvParameters)
{
  float theta_ref = M_PI/4.0;
  float phi_ref = 0;
  float l1_ini = 0.05;
  float l3_ini = 0.05;
  enableMotors();

  for(;;)
  {
    int n1 = heliosSection.length2steps(l1_ini-heliosSection.cableIKine(theta_ref, phi_ref, 1));
    int n3 = heliosSection.length2steps(l3_ini-heliosSection.cableIKine(theta_ref, phi_ref, 3));

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
  
  spi_sensor.begin(SCK, MISO, MOSI, SS0);
  spi_sensor.setClockDivider(SPI_CLOCK_DIV32);//divide the clock by 8

  int mainCore = xPortGetCoreID();

  xTaskCreatePinnedToCore(readSensors, "TaskSensors", 10000, NULL, 2, &task_sensors, !mainCore);
  delay(10);
  xTaskCreatePinnedToCore(mainLoop, "MainLoop", 10000, NULL, 2, &task_loop, mainCore);
}

void loop()
{
}