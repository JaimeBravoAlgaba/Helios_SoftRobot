#include <HeliosModule.h>

void heliosInit()
{
    pinMode(P0, INPUT);
    pinMode(P1, INPUT);
    pinMode(P2, INPUT);
    pinMode(P3, INPUT);
    pinMode(LED, OUTPUT);
}

HeliosData readSensors()
{
    HeliosData data;
    
    digitalWrite(LED, HIGH);
    delay(LED_ON_TIME);

    data.p0 = analogRead(P0);
    data.p1 = analogRead(P1);
    data.p2 = analogRead(P2);
    data.p3 = analogRead(P3);

    digitalWrite(LED, LOW);

    return data;
}