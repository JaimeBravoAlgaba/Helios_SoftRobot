#include <HeliosModule.h>

volatile byte command = 0x00;

HeliosData_union hd;

void setup () {
   initCommunications();
   heliosInit();
}

void loop () {
   hd.data = readSensors();
   delay(50);
}