#include <HeliosModule.h>

void heliosInit()
{
    pinMode(P0, INPUT);
    pinMode(P1, INPUT);
    pinMode(P2, INPUT);
    pinMode(P3, INPUT);
    pinMode(LED, OUTPUT);
    
    digitalWrite(LED, HIGH);
}

void readSensors(SensorData* hd)
{
    hd[0] = hd[0]*(MVA_PERIODS-1)/MVA_PERIODS + analogRead(P0)/MVA_PERIODS;
    hd[1] = hd[1]*(MVA_PERIODS-1)/MVA_PERIODS + analogRead(P1)/MVA_PERIODS;
    hd[2] = hd[2]*(MVA_PERIODS-1)/MVA_PERIODS + analogRead(P2)/MVA_PERIODS;
    hd[3] = hd[3]*(MVA_PERIODS-1)/MVA_PERIODS + analogRead(P3)/MVA_PERIODS;
}

void printSensors(SensorData* hd)
{
    // print out data
    Serial.print(hd[0]); Serial.print(',');
    Serial.print(hd[1]); Serial.print(',');
    Serial.print(hd[2]); Serial.print(',');
    Serial.println(hd[3]);
}