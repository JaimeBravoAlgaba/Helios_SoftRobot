#include <Arduino.h>
#include <SPI.h>

struct __attribute__((__packed__)) HeliosData
{
   uint16_t p0, p1, p2, p3;
};

HeliosData data;
const uint8_t numBytes = sizeof(data);

union HeliosData_union
{
   HeliosData data;   
   char data_bytes[sizeof(data)];
} hd;

void setup (void) {
   Serial.begin(9600); //set baud rate to 115200 for usart
   pinMode(SS, OUTPUT);
   digitalWrite(SS, HIGH); // disable Slave Select
   SPI.begin ();
   SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8
}

void loop (void) {
   byte command;

   command = 0x01;
   for (uint8_t i=0; i<numBytes; i++)
   {
      digitalWrite(SS, LOW); // enable Slave Select
      hd.data_bytes[i] = SPI.transfer (command);
      digitalWrite(SS, HIGH); // disable Slave Select
   }

   Serial.print(hd.data.p0); Serial.print(",");
   Serial.print(hd.data.p1); Serial.print(",");
   Serial.print(hd.data.p2); Serial.print(",");
   Serial.println(hd.data.p3);
   
   delay(100);
}