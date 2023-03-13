#include <SPI.h>
#include <HeliosModule.h>

volatile byte cmd;
volatile boolean cmd_available;
extern HeliosData_union hd;
volatile uint8_t index = 0;

void initCommunications()
{
    pinMode(SS,INPUT_PULLUP);
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
    SPDR = hd.data_bytes[index];
    index++;
    if (index >= sizeof(hd.data_bytes))
        index = 0;
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