#include <HeliosModule.h>

SensorData data[4] = {0, 0, 0, 0};     // Sample vector

void setup () {
   initCommunications();
   heliosInit();
}

void loop () {
   readSensors(data);
}