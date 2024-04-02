// BOARD: "DOIT ESP32 DEVKIT V1"

#include "HeliosPanel.h"

CoordsPCC ref = {0, 0};

void setup()
{
  Serial.begin(115200);
  initSensor();
  initActuator();
}

void loop()
{ 
  moveSection(SEC0, ref={M_PI/4, 0});
  delay(1000);
  moveSection(SEC0, ref={0, 0});
  delay(1000);
  moveSection(SEC0, ref={M_PI/4, M_PI});
  delay(1000);
  moveSection(SEC0, ref={0, 0});
  buzz(2, 128, 100, 100);
  delay(2000);
}