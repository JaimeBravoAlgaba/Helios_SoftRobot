#include <HeliosModule.h>
#include <SPI.h>
#include <Arduino.h>

SensorData data[4] = {0, 0, 0, 0};     // Sample vector
volatile uint8_t req;   // Request from master

void initCommunications()
{
    pinMode(SS,INPUT_PULLUP);
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE); // turn on SPI in slave mode
    SPI.setClockDivider(SPI_CLOCK_DIV64);
    SPI.attachInterrupt(); // turn on interrupt
}

ISR (SPI_STC_vect) // SPI interrupt routine 
{
  req = SPDR; // Read request from master

  // Look-Up Table
  if(req == SPI_OK)
    SPDR = SPI_OK;

  else if(req == SPI_REQ_A)
    SPDR = lowByte(data[0]);

  else if(req == SPI_REQ_A+1)
    SPDR = highByte(data[0]);

  else if(req == SPI_REQ_B)
    SPDR = lowByte(data[1]);

  else if(req == SPI_REQ_B+1)
    SPDR = highByte(data[1]);

  else if(req == SPI_REQ_C)
    SPDR = lowByte(data[2]);

  else if(req == SPI_REQ_C+1)
    SPDR = highByte(data[2]);

  else if(req == SPI_REQ_D)
    SPDR = lowByte(data[3]);

  else if(req == SPI_REQ_D+1)
    SPDR = highByte(data[3]);

  else
    SPDR = SPI_ERR;
}

void setup () {
   initCommunications();
   heliosInit();
}

void loop () {
   // Read sensors
   noInterrupts();
      readSensors(data);
   interrupts();
   delay(5);
}