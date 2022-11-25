#include <HeliosModule.h>

byte command = 0x00;

void setup () {
   Serial.begin (115200);
   initCommunications();
}

void loop () {
   if (commandAvailable()) {
      command = readCommand();

      switch(command)
      {
         case 0x01:
            Serial.println("Recibido comando 0x01");
            break;

         case 0x02:
            Serial.println("Recibido comando 0x02");
            break;

         default:
            Serial.println("Comando no valido");
            break;
      }
   }
}