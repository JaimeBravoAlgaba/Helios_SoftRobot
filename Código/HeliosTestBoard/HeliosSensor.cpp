#include "HeliosTestBoard.h"

HeliosSensor :: HeliosSensor(uint8_t ssPin)
{
  _ssPin = ssPin;
  _buffLen = sizeof(SensorData);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  pinMode(_ssPin, OUTPUT);
  digitalWrite(_ssPin, HIGH); // Inicialmente, deshabilitamos la comunicación SPI
}

uint8_t HeliosSensor :: reqByte(uint8_t msg)
{
  digitalWrite(_ssPin, LOW);  // Habilitamos la comunicación SPI con el esclavo
  uint8_t msg_back = SPI.transfer(msg);  // Leer la respuesta del esclavo
  digitalWrite(_ssPin, HIGH); // Deshabilitar la comunicación SPI con el esclavo
  delayMicroseconds(20);
  return msg_back;
}

uint8_t HeliosSensor :: reqData(uint8_t req, SensorData* data, uint8_t len, uint8_t ttl)
{
  uint8_t byte_array[len];
  byte_array[1] = reqByte(req);
  byte_array[0] = reqByte(req+1);   
  memcpy(data, byte_array, len);
  return SPI_OK;
}

void HeliosSensor :: update()
{
  SensorData aux[4];
  reqData(SPI_REQ_A, aux+0, _buffLen, 255);
  delay(1);
  reqData(SPI_REQ_B, aux+1, _buffLen, 255);
  delay(1);
  reqData(SPI_REQ_C, aux+2, _buffLen, 255);
  delay(1);
  reqData(SPI_REQ_D, aux+3, _buffLen, 255);

  for(uint8_t i=0; i<4; ++i)
  {
    // Reduce artifacts
    //aux[i] = (aux[i]>1023)?_currReading[i]:aux[i];
    //aux[i] = (aux[i]<0)?_currReading[i]:aux[i];

    _currReading[i] = aux[i];
  }
}

SensorData HeliosSensor :: getReading(uint8_t i)
{
  return _currReading[i];
}

void HeliosSensor :: print()
{
    Serial.print(0); Serial.print(",");
    for(uint8_t i=0; i<3; ++i)
    {
      Serial.print(_currReading[i]); Serial.print(",");
    }
    Serial.println(_currReading[3]);
}