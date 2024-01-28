#include <HeliosModule.h>

SensorData data[4] = {0, 0, 0, 0};     // Sample vector
ImuData imu_data;

AsyncDelay imu_read_interval;

void setup ()
{
   Serial.begin(9600);

   initCommunications();
   heliosInit();
   initIMU();   
   imu_read_interval.start(100, AsyncDelay::MILLIS);
}

void loop ()
{
   if (imu_read_interval.isExpired())
   {
      readIMU(&imu_data);
      //printIMU(&imu_data);
      imu_read_interval.restart();
   }

   else
   {
      readSensors(data);
      printSensors(data);
   }
}