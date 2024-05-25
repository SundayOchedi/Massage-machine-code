#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>

//weight scale
      NAU7802 myScale;
      float weightvalue;
 
// Initialize the LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Format -> (Address,Width,Height )

//linear actuator
            // Define control pins for H-Bridge
            const int extendPin = 11; // left (LPWM) Control pin to extend the actuator
            const int retractPin = 10; // right (RPWM) Control pin to retract the actuator

// Encoder pins
const int clkPin = 6; // CLK pin
const int dtPin = 7; // DT pin
const int buttonPin = 4; // Button pin

int massageWeight = 0;
int massageSpeed = 0; // 0 = Slow, 1 = Medium, 2 = Fast
int massageDuration = 0; // Duration in minutes
int startmassage=0;
unsigned long startTime = 0;
int massagebegin=0;
int massagetime=0;
long remainingTime;
long remainingTime2;
unsigned long currentTime;
unsigned long beginmassage;
unsigned long massageend;
int slow;
int medium;
int fast;
int currentStateCLK;
int lastStateCLK;
String currentMode = "weight"; // Start with setting weight
float weightkg;

//stepper motor 
          const int stepPin = 5;
          const int dirPin = 2;
          const int enPin = 8;
          const float distance =100; // Target distance in mm but it actually covers time 2 of the distance assigned here
          const float stepsPerMillimeter = 100; // Steps per millimeter - replace with your actual value
          float desiredRPM = 0; // Desired RPM - replace with your value
          unsigned long massagertime=0;
          long totalSteps;
          long stepDelay; 
          unsigned long singlecycle; //time to complete one cycle
          int numCycle; // number of massage cysle
          unsigned long prevStepMicros = 0;
          unsigned long microsBetweenSteps; // Microseconds


void setup() {
  lcd.begin(16, 2); // Set up the LCD's number of columns and rows
  Serial.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();

//stepper motor
        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
        pinMode(enPin, OUTPUT);
        digitalWrite(enPin, LOW); // Enable the driver

  //linear actuator
  // Initialize the control pins as outputs
  pinMode(extendPin, OUTPUT);
  pinMode(retractPin, OUTPUT);
  // Ensure actuator is not moving at startup
  stopActuator();

//weight sensor
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

//encoder button
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Enable internal pull-up
  
  lastStateCLK = digitalRead(clkPin); // Read the initial state of CLK

  lcd.print("Set weight");
  lcd.setCursor(0, 1);
  lcd.print(massageWeight);
}

void loop() {

  // Read the current state of CLK
  currentStateCLK = digitalRead(clkPin);


  // If the state of CLK has changed, a pulse has occurred
  if (currentStateCLK != lastStateCLK){
    // If the DT state is different than the CLK state, the encoder is rotating clockwise
    if (digitalRead(dtPin) != currentStateCLK) {
      if(currentMode == "weight") {
        massageWeight++;
      } else if(currentMode == "speed") {
        massageSpeed++;
        if(massageSpeed > 2) massageSpeed = 0; // Cycle through 0, 1, 2 (Slow, Medium, Fast)
      } else if(currentMode == "duration") {
        massageDuration++;
      }
       else if(currentMode == "startmassage") {
        startmassage++;
      }
    } else {
      if(currentMode == "weight") {
        massageWeight--;
      } else if(currentMode == "speed") {
        massageSpeed--;
        if(massageSpeed < 0) massageSpeed = 2; // Cycle through 2, 1, 0 (Fast, Medium, Slow)
      } else if(currentMode == "duration") {
        massageDuration--;
      }
      else if(currentMode == "startmassage") {
        startmassage--;
      }
    }
    updateDisplay();
  }
  lastStateCLK = currentStateCLK; // Update lastStateCLK with the current state

  // Check if button is pressed (LOW when pressed because of the internal pull-up)
  if (!digitalRead(buttonPin)){
    delay(200); // Debounce delay
    if(currentMode == "weight") {
      currentMode = "speed"; // Switch to setting speed
      lcd.clear();
      lcd.print("Set speed");
    } else if(currentMode == "speed") {
      currentMode = "duration"; // Switch to setting duration
      lcd.clear();
      lcd.print("Set duration");
    } else if(currentMode == "duration") {
      currentMode = "startmassage"; // Switch to setting duration
      lcd.clear();
      lcd.print("Start massage in:");
 
    }
  else if (currentMode == "startmassage") {
  currentMode = "beginmassage";
  lcd.clear();
  beginmassage = massagebegin * 60000; // Ensure massagebegin is set
  startTime = millis(); // Capture start time here
  // No need to print currentTime immediately after this
    }
  }
      if (currentMode == "beginmassage") {
    countdown();
     if (remainingTime <= 0) {
      currentMode = "massaging";
      massageend = massagetime * 60000; // Ensure massagebegin is set
      startTime = millis(); // Capture start time here
  }
  }
   if (currentMode == "massaging") {
               if(massageSpeed == 0){
             massageSpeed = 100;
           }
           else if(massageSpeed == 1){
             massageSpeed = 300;
           }
           else if (massageSpeed == 2 ){
             massageSpeed = 600;
           }

const float distance =80; // Target distance in mm but it actually covers time 2 of the distance assigned here
const float stepsPerMillimeter = 100; // Steps per millimeter - replace with your actual value
const float desiredRPM = massageSpeed;// Desired RPM - replace with your value
long massagertime=massagetime * 600000;
           // Calculate the total steps needed for the desired distance
            totalSteps = distance * stepsPerMillimeter;
           // Calculate the delay between steps in microseconds for the desired RPM
           const long stepDelay = 60L * 1000L * 1000L / (stepsPerMillimeter * desiredRPM);
           unsigned long singlecycle= stepDelay *2*totalSteps; //time to complete one cycle
           numCycle= massagertime/stepDelay; // number of massage cysle
           prevStepMicros = 0;
           microsBetweenSteps = stepDelay / 2; // Microseconds
           //weightkg =weightkg/2;
          float stopweight= weightkg - 0.2;
          float stopweight2= weightkg + 0.2;
          float moveweight= weightkg - 0.3;
          float moveweight2= weightkg + 0.3;
           countdown2();
while (remainingTime2 > 0) { // Continuous loop
           countdown2();
           if (remainingTime2 > 0){
    weightsensor();
    Serial.print("weightkg:");
    Serial.println(weightkg);
    Serial.print("massageSpeed:");
    Serial.println(massageSpeed);
    Serial.print("massagetime:");
    Serial.println(massagetime);
    Serial.print("massagebegin:");
    Serial.println(massagebegin);
             
             Serial.print("totalSteps: ");
             Serial.println(totalSteps);
             Serial.print("stepDelay: ");
             Serial.println(stepDelay);
             Serial.print("massagertime: ");
             Serial.println(massagertime);
             Serial.println("singlecycle: ");
             Serial.println(singlecycle);
             Serial.print("numcycle: ");
             Serial.println(numCycle);
             Serial.print("desiredRPM: ");
             Serial.println(desiredRPM);
             
                 // Linear actuator for control

                    if (weightvalue >= stopweight && weightvalue <= stopweight2){
                      //When weightvalue equals weightkg, the stopActuator() function halts any actuator movement, maintaining its position to stabilize at the target weight.
                      stopActuator();
                        Serial.println("stopactuator");
                        Serial.print("weightkg:");
                        Serial.println(weightkg);
                    }
                   if (weightvalue < moveweight){
                    //If weightvalue is less than weightkg, the extendActuator() function is called to extend the linear actuator arm forward, either to increase force or adjust for balance.
                    extendActuator(); //extends the linear actuator arm forward
                        Serial.println("extendactuator");
                        Serial.print("weightkg:");
                        Serial.println(weightkg);
                    }
                    if(weightvalue > moveweight2){
                      //If weightvalue is greater than weightkg, the retractActuator() function is activated to pull the actuator arm backward, reducing force or withdrawing from an object.
                      retractActuator(); // draws the actuator arm backwards
                        Serial.println("retractactuator");
                        Serial.print("weightkg:");
                        Serial.println(weightkg);
                    }

                    if(weightvalue >= stopweight && weightvalue <= stopweight2){
                    //control stepper motor                     
                      moveMotor(totalSteps, LOW); // Move forward
                      moveMotor(totalSteps, HIGH); // Move backward
                      digitalWrite(enPin, LOW);
                    }
           }
          else {
      break; // Exit the loop if remainingTime2 is 0 or less
    }
   }

    if (remainingTime2 <= 0) {
     // retractActuator();
      delay(300);
      currentMode = "weight";
      lcd.clear();
      lcd.print("Set weight");
    }
  }

}

//displace update function
      void updateDisplay() {
        lcd.clear();
        if(currentMode == "weight") {
          lcd.print("weight: ");
          weightkg=massageWeight*-0.5;
          lcd.print(weightkg);
          lcd.print("kg");
          lcd.setCursor(8, 1);
          lcd.print(weightkg*2.20462);
          lcd.print("lb");
        } else if(currentMode == "speed") {
          lcd.print("Speed: ");
          if(massageSpeed == 0) lcd.print("Slow");
          else if(massageSpeed == 1) lcd.print("Medium");
          else lcd.print("Fast");
        } else if(currentMode == "duration") {
          lcd.print("Duration: ");
          massagetime=massageDuration*-0.5;
          lcd.print(massagetime);
          lcd.print(" min");
        }
        else if(currentMode == "startmassage") {
          lcd.print("Start in: ");
          massagebegin=startmassage*-0.5;
          lcd.print(massagebegin);
          lcd.print(" min");
        }
      }

//countdown for massage start time
void countdown() {
  currentTime = millis(); // Update current time
  remainingTime = beginmassage - (currentTime - startTime);
  
  if (remainingTime > 0) {
    int minutes = remainingTime / 60000; // Convert to minutes
    int seconds = (remainingTime % 60000) / 1000; // Convert remainder to seconds

    // Update LCD less frequently and without blocking
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime >= 1000) { // Update every second
      lastUpdateTime = millis(); // Reset last update time
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Start massage in:");
      lcd.setCursor(0, 1);
      
      if (minutes < 10) lcd.print("0");
      lcd.print(minutes);
      lcd.print(":");
      if (seconds < 10) lcd.print("0");
      lcd.print(seconds);
    }
  } 
}

//countdown for massage end time
        void countdown2() {
          currentTime = millis(); // Update current time
          remainingTime2 = massageend - (currentTime - startTime);
          
          if (remainingTime2 > 0) {
            int minutes = remainingTime2 / 60000; // Convert to minutes
            int seconds = (remainingTime2 % 60000) / 1000; // Convert remainder to seconds

            // Update LCD less frequently and without blocking
            static unsigned long lastUpdateTime = 0;
            if (millis() - lastUpdateTime >= 1000) { // Update every second
              lastUpdateTime = millis(); // Reset last update time
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("massage ends in:");
              lcd.setCursor(0, 1);
              
              if (minutes < 10) lcd.print("0");
              lcd.print(minutes);
              lcd.print(":");
              if (seconds < 10) lcd.print("0");
              lcd.print(seconds);
            }
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Massage done");
            delay(3000);
          }
        }
// weight sensor funtions
void weightsensor(){
  if (myScale.available()) {
    weightvalue = myScale.getWeight(); // Get the weight in kg
    Serial.print("weightvalue: ");
    Serial.print(weightvalue, 3); // Print the weight with three decimal places
    Serial.println(" kg");
  }
}

//linear actuator functions
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


// stepper motor function

void moveMotor(long steps, int direction) {
  digitalWrite(dirPin, direction); // Set the direction
  for (long x = 0; x < steps; x++) {
    digitalWrite(stepPin, HIGH);
    singleStep(); // Adjust for the desired speed
    digitalWrite(stepPin, LOW);
    singleStep(); // Adjust for the desired speed
  }
}

void singleStep() {
  unsigned long currentMicros = micros();
  while (currentMicros - prevStepMicros < microsBetweenSteps) {
    currentMicros = micros(); // Continuously update currentMicros until the time has passed
  }
  prevStepMicros = currentMicros;
}
