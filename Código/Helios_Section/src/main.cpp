#include <HeliosModule.h>

volatile byte command = 0x00;

HeliosData_union hd;

void setup () {
   initCommunications();
   heliosInit();
}

void loop () {
   if(digitalRead(SS))
   {  
      hd.data = readSensors();
      delay(20);
   }
}