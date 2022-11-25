#include <Arduino.h>
#include <SPI.h>

void setup (void) {
   Serial.begin(115200); //set baud rate to 115200 for usart
   digitalWrite(SS, HIGH); // disable Slave Select
   SPI.begin ();
   SPI.setClockDivider(SPI_CLOCK_DIV8);//divide the clock by 8
}

void loop (void) {
   byte command;

   command = 0x01;
   digitalWrite(SS, LOW); // enable Slave Select
   SPI.transfer (command);      
   digitalWrite(SS, HIGH); // disable Slave Select
   delay(2000);

   command = 0x02;
   digitalWrite(SS, LOW); // enable Slave Select
   SPI.transfer (command);      
   digitalWrite(SS, HIGH); // disable Slave Select
   delay(2000);

   command = 0x03;
   digitalWrite(SS, LOW); // enable Slave Select
   SPI.transfer (command);      
   digitalWrite(SS, HIGH); // disable Slave Select
   delay(2000);
}