#include <Wire.h>
#include "CNC_Shield_UNO.h"
#include "Helios.h"

#define HELIOS_ADDR 0x4A
#define IMU_0_ADDR 0x08
#define TCA_ADDR 0x70

HeliosSensor helios;
Adafruit_VL6180X tof = Adafruit_VL6180X();

uint32_t h[4] = {0, 0, 0, 0}; // Helios module measurement
uint8_t l_tofs[4]; // TOF modules measurement

float cableLengths[4] = {SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN};
sensors_event_t orientationData;

CoordsPCC coords_ref;
CoordsPCC coords_meas;

bool succ_init = true;

void move(CoordsPCC c) {
  float l_ref[4] = {0, 0, 0, 0};
  long dn[4] = {0, 0, 0, 0};
  
  for (uint8_t i = 0; i < 4; ++i) {
    l_ref[i] = cableIKine(c, i);
    dn[i] = length2steps(l_ref[i] - cableLengths[i]);
    cableLengths[i] = l_ref[i];
  }
  stepParallel(dn);
}

void calibrateCables() {
  print_info("Starting calibration... ");
  
  // 1. De-tension
  long n = length2steps(0.007); // Lengthen the cables by 5mm
  long dn[4] = {n, n, n, n};
  stepParallel(dn);
  delay(500);

  for (uint8_t i = 0; i < 4; ++i) {
    dn[i] = 0;
  }

  // 2. Tension loop
  n = length2steps(-0.00025); // Shorten each cable by 1mm per cycle
  for (uint8_t i = 0; i < 4; ++i) {
    h[i] = readHelios(i);
    int thr = 0.007 * h[i];
    uint32_t u = h[i];
    dn[i] = n;
    
    while (abs((int)h[i] - (int)u) < thr) {
      stepParallel(dn);
      u = readHelios(i);
    }
    cableLengths[i] = SEGMENTS_LEN;
    dn[i] = 0;
    delay(100);
  }

  print_info("OK\n");
}

void printData() {
  // Print TOF data
  for (uint8_t i = 0; i < 4; ++i) {
    Serial.print(l_tofs[i]);
    Serial.print(",");
  }

  // Print Helios data
  for (uint8_t i = 0; i < 4; ++i) {
    Serial.print(h[i]);
    if (i < 3) {
      Serial.print(",");
    } else {
      Serial.print("\n");
    }
  }
}

void setup() {
  setupCNC();

  Serial.begin(115200);
  while (!Serial) delay(10);  // Wait for serial port to open
  Wire.begin();

  // HELIOS INIT
  print_info("Initializing Helios module on address ");
  print_info(String(HELIOS_ADDR));
  print_info("\n");
  while (!helios.begin(HELIOS_ADDR)) {
    print_info(F("Helios Module not detected. Retrying...\n"));
    delay(500);
  }

  helios.configureADCmode(ADS122C04_RAW_MODE);
  helios.setGain(ADS122C04_GAIN_128);
  helios.setConversionMode(ADS122C04_CONVERSION_MODE_CONTINUOUS);
  helios.start();

  print_info("OK\n");

  // TCA9548A Initialization
  print_info("Initializing TCA9548A with address ");
  print_info(String(TCA_ADDR));
  print_info("\n");
  
  for (uint8_t i = 0; i < 4; ++i) {
    tcaSelect(i);
    if (!tof.begin()) {
      print_info(F("\tFailed to boot TOF on channel "));
      print_info(String(i));
      print_info("\n");
      succ_init = false;
    } else {
      print_info(F("\tSuccess booting TOF on channel "));
      print_info(String(i));
      print_info("\n");
    }
  }
  print_info("OK\n");

  if (succ_init) {  
    print_info("\nInitialization successful!\n");
  } else {
    print_info("\nInitialization failed!\n");
    while (1);
  }

  print_info("theta, phi, L, h0, h1, h2, h3\n");
}

void loop() {  
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');

    if (cmd == "stop") {
      return;
    }
    
    else if (cmd == "calibrate") {
      calibrateCables();
    }
    
    else {
      float l0, l1, l2, l3;
      
      int firstComma = cmd.indexOf(',');
      int secondComma = cmd.indexOf(',', firstComma + 1);
      int thirdComma = cmd.indexOf(',', secondComma + 1);

      l0 = cmd.substring(0, firstComma).toFloat();
      l1 = cmd.substring(firstComma + 1, secondComma).toFloat();
      l2 = cmd.substring(secondComma + 1, thirdComma).toFloat();
      l3 = cmd.substring(thirdComma + 1).toFloat();

      Serial.print(l0, 4); Serial.print(","); Serial.print(l1, 4); Serial.print(","); Serial.print(l2, 4); Serial.print(","); Serial.print(l3, 4); Serial.print("\n");

      long dn[4] = {length2steps(l0), length2steps(l1), length2steps(l2), length2steps(l3)};
      stepParallel(dn);
      
      memmove(l_tofs, readTOFs(), sizeof(l_tofs) * sizeof(uint8_t));
      for (uint8_t i = 0; i < 4; ++i) {
        h[i] = readHelios(i);
      }
      printData();
    }
  }
}
