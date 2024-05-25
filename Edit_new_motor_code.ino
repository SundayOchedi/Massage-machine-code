const int stepPin = 5;
const int dirPin = 2;
const int enPin = 8;
const float distance =100; // Target distance in mm but it actually covers time 2 of the distance assigned here
const float stepsPerMillimeter = 100; // Steps per millimeter - replace with your actual value
const float desiredRPM = 6; // Desired RPM - replace with your value
long massagetime=600000;

// Calculate the total steps needed for the desired distance
const long totalSteps = distance * stepsPerMillimeter;

// Calculate the delay between steps in microseconds for the desired RPM
const long stepDelay = 60L * 1000L * 1000L / (stepsPerMillimeter * desiredRPM);
unsigned long singlecycle= stepDelay *totalSteps; //time to complete one cycle
long numcycle= massagetime/stepDelay; // number of massage cysle

void setup() {
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW); // Enable the driver
}

void loop() {                                                  
  Serial.println("totalSteps");
  Serial.println(totalSteps);
  Serial.println(stepDelay);
  Serial.println(singlecycle);
  Serial.println(numcycle);
  for (int i = 0; i < numcycle; i++) { // Example: Repeat the movement 5 times
    moveMotor(totalSteps, HIGH); // Move forward
    delay(500); // Short delay between directions
    moveMotor(totalSteps, LOW); // Move backward
    delay(500); // Short delay before the next cycle
  }
    digitalWrite(enPin, LOW);

  // After completing the movements, you might want to add a longer delay or stop the program.
  // delay(10000); // Delay 10 seconds before repeating the entire process
  // while(true); // Stop any further action
}

void moveMotor(long steps, int direction) {
  digitalWrite(dirPin, direction); // Set the direction
  for (long x = 0; x < steps; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay / 2); // Adjust for the desired speed
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay / 2); // Adjust for the desired speed
  }
}
                                                         