#include <Wire.h>
#include "CNC_Shield_UNO.h"
#include "Helios.h"

#define HELIOS_ADDR 0x4A
#define IMU_0_ADDR 0x08
#define TCA_ADDR 0x70

HeliosSensor helios;
Adafruit_VL6180X tof = Adafruit_VL6180X();

uint32_t h[4] = {0, 0, 0, 0}; // Helios module measurement
uint8_t l_tofs[4] = {0, 0, 0, 0}; // TOF modules measurement

float cableLengths[4] = {SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN, SEGMENTS_LEN};
sensors_event_t orientationData;

CoordsPCC coords_ref;
CoordsPCC coords_meas;

bool succ_init = true;

uint32_t readHelios(uint8_t i) {
  helios.setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS + i);
  delay(50);

  float n_samples = 20;
  float n_filter = 5.0;
  uint32_t h = 0;

  for (uint8_t n = 0; n < n_samples; ++n) {
    h = h * (n_filter-1) / n_filter + helios.readADC() / n_filter;
  }

  return h;
}

void tcaSelect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void readTOFs() {
  float n_samples = 20;
  float n_filter = 5.0;
  uint8_t l_tofs_aux[4] = {0, 0, 0, 0};

  for (uint8_t n = 0; n < n_samples; ++n) {
    for (uint8_t i = 0; i < 4; ++i) {
      tcaSelect(i);
      l_tofs_aux[i] = l_tofs_aux[i] * (n_filter-1) / n_filter + tof.readRange() / n_filter;
    }
  }

  for (uint8_t i = 0; i < 4; ++i) {
    l_tofs[i] = l_tofs_aux[i];
  }
}

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

  calibrateCables();

  print_info("theta, phi, L, h0, h1, h2, h3\n");
}

void loop() {  
  coords_ref.theta = 0;
  coords_ref.phi = 0;
  coords_ref.length = SEGMENTS_LEN;

  for (uint8_t j = 0; j < 8; ++j, coords_ref.theta = 0, coords_ref.phi += PI / 4.0) {
    for (uint8_t i = 0; i < 8; ++i, coords_ref.theta += PI / 28.0 * 1.1) {
      move(coords_ref);

      // Read Sensors
      for (uint8_t i = 0; i < 4; ++i) {
        h[i] = readHelios(i);
      }

      readTOFs();
      coords_meas = tofs2pcc(l_tofs[0], l_tofs[1], l_tofs[2], l_tofs[3]);
    
      printData();

      delay(50);
    }

    for (uint8_t i = 0; i < 16; ++i, coords_ref.theta -= PI / 28.0 * 1.1) {
      move(coords_ref);

      // Read Sensors
      for (uint8_t i = 0; i < 4; ++i) {
        h[i] = readHelios(i);
      }

      readTOFs();
      coords_meas = tofs2pcc(l_tofs[0], l_tofs[1], l_tofs[2], l_tofs[3]);
    
      printData();

      delay(50);
    }
    
    for (uint8_t i = 0; i < 8; ++i, coords_ref.theta += PI / 28.0 * 1.1) {
      move(coords_ref);

      // Read Sensors
      for (uint8_t i = 0; i < 4; ++i) {
        h[i] = readHelios(i);
      }

      readTOFs();
      coords_meas = tofs2pcc(l_tofs[0], l_tofs[1], l_tofs[2], l_tofs[3]);
    
      printData();

      delay(50);
    }

    coords_ref.theta = 0;
    move(coords_ref);
  }

  coords_ref.theta = 0;
  coords_ref.phi = 0;
  move(coords_ref);
  
  Serial.println("Test complete!");
  while(1);
}
