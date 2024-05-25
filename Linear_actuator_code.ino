
//linear actuator
// Define control pins for H-Bridge
const int extendPin = 11; // left (LPWM) Control pin to extend the actuator
const int retractPin = 10; // right (RPWM) Control pin to retract the actuator



void setup() {
  Serial.begin(115200);

  //linear actuator
  // Initialize the control pins as outputs
  pinMode(extendPin, OUTPUT);
  pinMode(retractPin, OUTPUT);
  // Ensure actuator is not moving at startup
  stopActuator();

}

void loop() {
  // Extend the actuator for 5 seconds
    // Retract the actuator for 5 seconds
  // retractActuator();
  //   delay(1000); // Wait 5 seconds
  // extendActuator();
  // delay(5000); // Wait 5 seconds

  stopActuator();
  delay (20000);
}

void retractActuator() {
  analogWrite(retractPin, 255); // Activate retract control
  analogWrite(extendPin, 0); // Ensure extend is inactive
}

void extendActuator() {
  analogWrite(retractPin, 0); // Ensure retract is inactive
  analogWrite(extendPin, 255); // Activate extend control
}

void stopActuator() {
  digitalWrite(extendPin, LOW); // Deactivate extend control
  digitalWrite(retractPin, LOW); // Deactivate retract control
}
