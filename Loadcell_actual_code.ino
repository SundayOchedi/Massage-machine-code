#include <Wire.h>
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>

NAU7802 myScale;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!myScale.begin()) {
    Serial.println("NAU7802 not detected. Please check wiring. Freezing...");
    while (1);
  }

  // No longer waiting for 't' input to tare
  // Directly setting zero offset
  long zeroOffset = 10; // Replace X with your zero offset value
  myScale.setZeroOffset(zeroOffset);

  // No longer waiting for 'c' input to calibrate
  // Directly setting calibration factor
  float calibrationFactor = 334071.00; // Replace Y with your calibration factor value
  myScale.setCalibrationFactor(calibrationFactor);

  Serial.println("Scale is calibrated using predefined values.");
}

void loop() {
  if (myScale.available()) {
    float weight = myScale.getWeight(); // Get the weight in kg
    Serial.print("Weight: ");
    Serial.print(weight, 3); // Print the weight with three decimal places
    Serial.println(" kg");
    delay(1000);
  }
}
