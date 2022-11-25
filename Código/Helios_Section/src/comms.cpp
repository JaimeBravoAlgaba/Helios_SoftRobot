#include <SPI.h>

volatile byte cmd;
volatile boolean cmd_available;

void initCommunications()
{
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE); // turn on SPI in slave mode
    cmd = 0x00;
    cmd_available = false;
    SPI.attachInterrupt(); // turn on interrupt
}

ISR (SPI_STC_vect) // SPI interrupt routine 
{ 
    cmd_available = true;
    cmd = SPDR; // read byte from SPI Data Register
}

boolean commandAvailable()
{
    return cmd_available;
}

byte readCommand()
{
    cmd_available = false;
    return cmd;
}