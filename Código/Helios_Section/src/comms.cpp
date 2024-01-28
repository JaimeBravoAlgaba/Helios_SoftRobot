#include <HeliosModule.h>

extern SensorData data[4];

void initCommunications()
{
   Wire.begin(I2C_ADDR);
   Wire.onRequest(requestEvent);
}

void requestEvent()
{
  Wire.write((byte*)data, sizeof(data));
}