
#include <LiquidCrystal_I2C.h>
 
// Initialize the LCD 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Format -> (Address,Width,Height )


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

void setup() {
  //lcd.begin(16, 2); // Set up the LCD's number of columns and rows
  lcd.init();
  lcd.backlight();

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
    countdown2();
    if (remainingTime <= 0) {
      currentMode = "weight";
      lcd.clear();
      lcd.print("Set weight");
    }
  }
}

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
void countdown2() {
  currentTime = millis(); // Update current time
  remainingTime = massageend - (currentTime - startTime);
  
  if (remainingTime > 0) {
    int minutes = remainingTime / 60000; // Convert to minutes
    int seconds = (remainingTime % 60000) / 1000; // Convert remainder to seconds

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