#include <Wire.h>
#include "CNC_Shield_UNO.h"
#include "Helios.h"

#define HELIOS_ADDR 0x48
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

  Serial.println("START");
}

void loop() {  
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');

    if (cmd == "HELLO") {
      Serial.println("OK");
      return;
    }
    
    else if (cmd == "CALIBRATE") {
      calibrateCables();
      Serial.println("OK");
    }
    
    else {
      float dl[4];
      long dn[4];

      if (cmd.startsWith("DELTA_LEN:")) {
        cmd.remove(0, String("DELTA_LEN:").length());

        int firstComma  = cmd.indexOf(',');
        int secondComma = cmd.indexOf(',', firstComma + 1);
        int thirdComma  = cmd.indexOf(',', secondComma + 1);

        int commas[3];
        commas[0] = cmd.indexOf(',');        
        commas[1] = cmd.indexOf(',', commas[0] + 1);
        commas[2] = cmd.indexOf(',', commas[1] + 1);

        dl[0] = cmd.substring(0, commas[0]).toFloat();
        dn[0] = length2steps(dl[0]);
        for (int i = 1; i < 4; ++i) {
          dl[i] = cmd.substring(commas[i - 1] + 1, (i < 3) ? commas[i] : cmd.length()).toFloat();
          dn[i] = length2steps(dl[i]);
          cableLengths[i] = dl[i] + cableLengths[i];
        }
      }

      else if (cmd.startsWith("DELTA_STEPS:")) {
        cmd.remove(0, String("DELTA_STEPS:").length());

        int firstComma  = cmd.indexOf(',');
        int secondComma = cmd.indexOf(',', firstComma + 1);
        int thirdComma  = cmd.indexOf(',', secondComma + 1);
        
        int commas[3];
        commas[0] = cmd.indexOf(',');
        commas[1] = cmd.indexOf(',', commas[0] + 1);
        commas[2] = cmd.indexOf(',', commas[1] + 1);

        dn[0] = cmd.substring(0, commas[0]).toInt();
        for (int i = 1; i < 4; ++i) {
          dn[i] = cmd.substring(commas[i - 1] + 1, (i < 3) ? commas[i] : cmd.length()).toInt();
          dl[i] = steps2length(dn[i]);
          cableLengths[i] = dl[i] + cableLengths[i];
        }        
      }

      else if (cmd.startsWith("REF_PCC:")) {
        cmd.remove(0, String("REF_PCC:").length());

        int firstComma  = cmd.indexOf(',');
        int secondComma = cmd.indexOf(',', firstComma + 1);
        int thirdComma  = cmd.indexOf(',', secondComma + 1);

        coords_ref.theta = cmd.substring(0, firstComma).toFloat();
        coords_ref.phi = cmd.substring(firstComma + 1, secondComma).toFloat();
        coords_ref.length = cmd.substring(secondComma + 1, thirdComma).toFloat();
        
        for (uint8_t i = 0; i < 4; ++i) {
          dl[i] = cableIKine(coords_ref, i) - cableLengths[i];
          dn[i] = length2steps(dl[i]);
          cableLengths[i] = dl[i] + cableLengths[i];
        }
      }
      else {
        Serial.println("ERROR");
        return;
      }
      
      stepParallel(dn);
      
      memmove(l_tofs, readTOFs(), sizeof(l_tofs) * sizeof(uint8_t));
      for (uint8_t i = 0; i < 4; ++i) {
        h[i] = readHelios(i);
      }
      printData();
    }
  }
}
