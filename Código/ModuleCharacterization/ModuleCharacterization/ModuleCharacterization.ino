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
  
  // De-tension the cables lengthening them
  long dn[4] = {length2steps(0.005), length2steps(0.005), length2steps(0.005), length2steps(0.005)};
  stepParallel(dn);
  
  memmove(l_tofs, readTOFs(), sizeof(l_tofs) * sizeof(uint8_t));
  long l_tofs_init[4];
  for (uint8_t i = 0; i < 4; ++i) {
    dn[i] = 0;
  }
  
  for(uint8_t i = 0; i < 4; ++i) {
    dn[i] = length2steps(-0.001);
    l_tofs_init[i] = l_tofs[i];

    while (abs(l_tofs[i] - l_tofs_init[i]) < 3) {
      stepParallel(dn);
      memmove(l_tofs, readTOFs(), sizeof(l_tofs) * sizeof(uint8_t));
    }
    dn[i] = 0;
  }

  coords_meas = tofs2pcc(l_tofs[0], l_tofs[1], l_tofs[2], l_tofs[3]);
}

void printData() {
  // Print TOF data
  Serial.print("{\"TOFS\":[");
  for (uint8_t i = 0; i < 3; ++i) {
    Serial.print(l_tofs[i]);
    Serial.print(",");
  }
  Serial.print(l_tofs[3]);

  // Print Helios data
  Serial.print("], \"HELIOS\":[");
  for (uint8_t i = 0; i < 4; ++i) {
    Serial.print(h[i]);
    if (i < 3) {
      Serial.print(",");
    } else {
      Serial.print("]}\n");
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

      else if (cmd.startsWith("DELTA_PCC:")) {
        cmd.remove(0, String("DELTA_PCC:").length());

        int firstComma  = cmd.indexOf(',');
        int secondComma = cmd.indexOf(',', firstComma + 1);
        int thirdComma  = cmd.indexOf(',', secondComma + 1);

        coords_ref.theta = cmd.substring(0, firstComma).toFloat() + coords_meas.theta;
        coords_ref.phi = cmd.substring(firstComma + 1, secondComma).toFloat() + coords_meas.phi;
        coords_ref.length = cmd.substring(secondComma + 1, thirdComma).toFloat() + coords_meas.length;
        
        for (uint8_t i = 0; i < 4; ++i) {
          dl[i] = cableIKine(coords_ref, i) - cableLengths[i];
          dn[i] = length2steps(dl[i]);
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

      else if (cmd.startswith("REF_LEN:")) {
        cmd.remove(0, String("REF_LEN:").length());

        int firstComma  = cmd.indexOf(',');
        int secondComma = cmd.indexOf(',', firstComma + 1);
        int thirdComma  = cmd.indexOf(',', secondComma + 1);

        dl[0] = cmd.substring(0, firstComma).toFloat() - cableLengths[0];
        dn[0] = length2steps(dl[0]);
        for (uint8_t i = 1; i < 4; ++i) {
          dl[i] = cmd.substring(commas[i - 1] + 1, (i < 3) ? commas[i] : cmd.length()).toFloat() - cableLengths[i];
          dn[i] = length2steps(dl[i]);
          cableLengths[i] = dl[i] + cableLengths[i];
        }
      }
      else {
        Serial.println("ERROR");
        return;
      }
      
      stepParallel(dn);
      Serial.println("OK");
        }
      }
      
  memmove(l_tofs, readTOFs(), sizeof(l_tofs) * sizeof(uint8_t));
  coords_meas = tofs2pcc(l_tofs[0], l_tofs[1], l_tofs[2], l_tofs[3]);
  for (uint8_t i = 0; i < 4; ++i) {
    h[i] = readHelios(i);
  }
  printData();
}
