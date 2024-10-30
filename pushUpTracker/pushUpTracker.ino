#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PulseSensorPlayground.h>

#define USE_ARDUINO_INTERRUPTS true

// Pin configuration for keypad
const int rowPins[4] = {2, 3, 4, 5};
const int colPins[4] = {6, 7, 8, 9};

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic sensor configuration
const int trigPin = 10;
const int echoPin = 11;
long echoDuration;
int distance;

// Pulse sensor configuration
const int PulseWire = 0;
const int LED13 = 13;
int Threshold = 550;
PulseSensorPlayground pulseSensor;

// Timer variables
unsigned long startMillis;
unsigned long currentMillis;
long timeCount = 0;

// Push-up variables
const int upDistance = 40;
const int downDistance = 25;
bool isDown = false;
int pushUpCount = 0;

// State
bool selectModeState = true;
bool pushupState = false;
bool resultState = false;
bool heartRateState = false;
bool inputState = false;

// Mode
bool timeCountdownMode = true;
bool pushCountdownMode = false;

char inputString[6]; 
int remaining = 0;

void setup() {
    Serial.begin(9600);

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("PushUp Tracker");
    delay(1000);
    lcd.clear();

    // Set up ultrasonic sensor
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // Set up pulse sensor
    pulseSensor.analogInput(PulseWire);
    pulseSensor.blinkOnPulse(LED13);
    pulseSensor.setThreshold(Threshold);
    if (pulseSensor.begin()) {
        Serial.println("PulseSensor ready!");
    }

    // Set up keypad pins
    for (int i = 0; i < 4; i++) {
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);
    }
    for (int i = 0; i < 4; i++) {
        pinMode(colPins[i], INPUT_PULLUP);
    }
}

void loop() {

  if (selectModeState){
    selectMode();
    displayMode();
    delay(200);
  }

  if (inputState){
    input();
    displayInput();  
    delay(100);
  }

  if (pushupState){
    displayCount(startMillis, remaining);
    delay(200);
  }

  if (resultState){
    displayResult();
  }

  if (heartRateState){
    displayHeartRate();
  }
}

void displayMode(){

  lcd.setCursor(0, 0);
  lcd.print("Select Mode:");

  if(timeCountdownMode){
    clearSecondLine();
    lcd.setCursor(0, 1);
    lcd.print("Time Countdown");
  }else if(pushCountdownMode){
    clearSecondLine();
    lcd.setCursor(0, 1);
    lcd.print("PushUp Countdown");
  }
}

void selectMode(){
  char key = getKeyPressed();

  if (key == '*') {
    // Switch mode
    timeCountdownMode = !timeCountdownMode;
    pushCountdownMode = !pushCountdownMode;
  } else if (key == '#') {
    // Confirm
    selectModeState = false;
    inputState = true;
    lcd.clear();
  } else if (key == 'D'){
    // Reset
    NVIC_SystemReset();
  }
}

void displayInput(){
  if(timeCountdownMode){
    lcd.setCursor(0, 0);
    lcd.print("Input time: "); 
  }
    
  if(pushCountdownMode){
    lcd.setCursor(0, 0);
    lcd.print("Input count: ");
  }

  clearSecondLine();
  lcd.setCursor(0, 1);
  lcd.print(inputString);
}
    
void input(){
  char key = getKeyPressed();

  if (strlen(inputString)<5){
    if(key == '1') {
      inputString[strlen(inputString)] = '1';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '2') {
      inputString[strlen(inputString)] = '2';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '3') {
      inputString[strlen(inputString)] = '3';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '4') {
      inputString[strlen(inputString)] = '4';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '5') {
      inputString[strlen(inputString)] = '5';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '6') {
      inputString[strlen(inputString)] = '6';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '7') {
      inputString[strlen(inputString)] = '7';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '8') {
      inputString[strlen(inputString)] = '8';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '9') {
      inputString[strlen(inputString)] = '9';
      inputString[strlen(inputString)] = '\0';
    }else if(key == '0') {
      inputString[strlen(inputString)] = '0';
      inputString[strlen(inputString)] = '\0';
    }
  }

  if (key == '*' && strlen(inputString)>0){
    inputString[strlen(inputString)-1] = '\0';
  }
    
  if (key == '#' && strlen(inputString)!=0) {
    remaining = atoi(inputString);
    strcpy(inputString, "");
    startMillis = millis();
    inputState = false;
    pushupState = true;
  }

  if (key == 'D'){
    NVIC_SystemReset();
  }
}   

void displayCount(long startTime, int remaining){
  currentMillis = millis();
  timeCount = floor((currentMillis-startMillis)/1000);

  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
    
  
  // Set the trigPin HIGH for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
    
  
  // Read the echoPin, return the sound wave travel time in microseconds
  echoDuration = pulseIn(echoPin, HIGH);
    
  // Calculate the distance in cm and inches
  distance = echoDuration * 0.034 / 2;
  if (distance<=downDistance && !isDown){
    isDown = true;
  }
  if (isDown && (distance >= upDistance)){
    lcd.setCursor(11, 1);
    lcd.print("     ");
    isDown = false;
    pushUpCount++;
  }
  

  if(timeCountdownMode){
    
    remaining-=timeCount;

    lcd.setCursor(0, 0);
    lcd.print("Time Left: ");
    lcd.setCursor(11, 0);
    lcd.print("       ");
    lcd.setCursor(11, 0);
    lcd.print(remaining);
      
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.setCursor(7, 1);
    lcd.print("       ");
    lcd.setCursor(7, 1);
    lcd.print(pushUpCount);
  } 
  else if(pushCountdownMode){

    remaining-=pushUpCount;

    lcd.setCursor(0, 0);
    lcd.print("Time Count: ");
    lcd.setCursor(12, 0);
    lcd.print("       ");
    lcd.setCursor(12, 0);
    lcd.print(timeCount);
      
    lcd.setCursor(0, 1);
    lcd.print("Count Left: ");
    lcd.setCursor(12, 1);
    lcd.print("       ");
    lcd.setCursor(12, 1);
    lcd.print(remaining);
  }

  if (remaining<=0){
      isDown = false;
      lcd.clear();
      pushupState = false;
      resultState = true;
  }
  
  if (isDown){
    lcd.setCursor(11, 1);
    lcd.print("GO UP");
  }
  	
  char key = getKeyPressed();
  if (key == 'D'){
    NVIC_SystemReset();
  }
}

void displayResult(){

  if (timeCountdownMode){
    lcd.setCursor(0, 0);
    lcd.print("Time's up!");
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(pushUpCount);
  } else if (pushCountdownMode){
    lcd.setCursor(0, 0);
    lcd.print("Complete!");
    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(timeCount);
  }
     
  char key = getKeyPressed();
  if (key == '#') {
    resultState = false;
    heartRateState = true;
  } else if (key == 'D'){
    NVIC_SystemReset();
  }

}

void displayHeartRate(){

  int myBPM = pulseSensor.getBeatsPerMinute();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BPM: ");
  lcd.print(myBPM);
  delay(500);
    

  char key = getKeyPressed();

  if (key == '#') {
    timeCountdownMode = true;
    pushCountdownMode = false;
    remaining = 0;
    pushUpCount = 0;
      	
    selectModeState = true;
    heartRateState = false;
    lcd.setCursor(0, 0);
    lcd.print("PushUp Tracker");
    delay(2000);
    lcd.clear();

  } else if (key == 'D'){
    NVIC_SystemReset();
  }

}

char getKey(int row, int col) {

    char keys[4][4] = {
        {'D', 'C', 'B', 'A'},
        {'#', '9', '6', '3'},
        {'0', '8', '5', '2'},
        {'*', '7', '4', '1'}
    };

    return keys[row][col];
}

char getKeyPressed(){

  for (int row = 0; row < 4; row++) {
        digitalWrite(rowPins[row], LOW);
        for (int col = 0; col < 4; col++) {
            if (digitalRead(colPins[col]) == LOW) {
                char keyPressed = getKey(row, col);

                // Wait for button release to prevent multiple reads
                while (digitalRead(colPins[col]) == LOW);
                return keyPressed;
            }
        }
        digitalWrite(rowPins[row], HIGH);
    }
}

void clearSecondLine(){
  lcd.setCursor(0, 1);
  lcd.print("                ");
}
