#include <Arduino.h>
#include <SPI.h>
#include <motion.h>

struct HeliosData
{
   uint16_t p0, p1, p2, p3;
};

const uint8_t numBytes = sizeof(HeliosData);
bool dir = 1;

union HeliosData_union
{
   HeliosData data;   
   char data_bytes[sizeof(data)];
} hd;

void setup (void) {
   Serial.begin(9600);

   pinMode(SS, OUTPUT);
   digitalWrite(SS, HIGH); // disable Slave Select

   SPI.begin ();
   SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8
   
   testBenchInit();
   digitalWrite(STP_EN, LOW);
}

void loop (void) {
   for (int i = 0; i<80; ++i)
   {
      for(int k=0; k<50; ++k)
      {
         moveStepper('A', 1, dir, 75);
         moveStepper('C', 1, !dir, 75);
      }
      
      // READ SENSORS
      byte command;

      command = 0x01;
      for (uint8_t j=0; j<numBytes; j++)
      {
         digitalWrite(SS, LOW); // enable Slave Select
         hd.data_bytes[j] = SPI.transfer (command);
         digitalWrite(SS, HIGH); // disable Slave Select
      }

      // PLOTTING
      Serial.print((int)hd.data.p0); Serial.print(",");
      Serial.print((int)hd.data.p1); Serial.print(",");
      Serial.print((int)hd.data.p2); Serial.print(",");
      Serial.println((int)hd.data.p3);
   }

   dir = !dir;
}