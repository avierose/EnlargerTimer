#include <elapsedMillis.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

elapsedMillis timeElapsed;
unsigned int millisResolution = 100;
int backlightPin = 3;
int backlightVal = 60;
int currentTimeValue[] = {0, 0, 0, 5, 0};
int cdrun = 1;
int currentState = 1;
int focusFlag = 1;
int controlPin = 4;

//define the keypad
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {

  {1, 2, 3, 'A'},
  {4, 5, 6, 'B'},
  {7, 8, 9, 'C'},
  {'*', 0, '#', 'D'}
};

byte rowPins[rows] = {5, 6, 7, 8};
byte colPins[cols] = {9, 10, 11, 12};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);
  pinMode(controlPin, OUTPUT);
  relayStatus(false);

    //Initialize the lcd
  lcd.init(); 

  //Welcome Screen.
  lcd.backlight();
  analogWrite(backlightPin, backlightVal);

  lcd.clear();
//  lcd.createChar(0, ghosty);
  lcd.setCursor(0, 0);
  lcd.print(" BonsaiMadTimer ");
  lcd.setCursor(0, 1);
//  lcd.print(char(0));
//  lcd.print(" " + versionNumber + " ");
//  lcd.print(char(0));

  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
//  lcd.print(char(0));
  lcd.print(" Leigh Works ");
//  lcd.print(char(0));
  lcd.setCursor(0, 1);
  lcd.print("Durst M370");

  delay(2000);

    //display main screen
  lcd.clear();
  dispTimeEntry();

  //setup default time to 00:00.0
  currentTimeValue[0] = 0;
  currentTimeValue[1] = 0;
  currentTimeValue[2] = 0;
  currentTimeValue[3] = 0;
  currentTimeValue[4] = 0;
  showEnteredTime();
}

void loop() {
 int l;
  char tempVal[6];
  char key = keypad.getKey();

  //key pressed and state is 1
  if (int(key) != 0 and currentState == 1) {

    switch (key) {
      // Manual override of the relay when not counting down for focus purposes
      case 'A': 

        //Focus button pressed, englarger powered
        if ((focusFlag) == 1) { 
          
          relayStatus(true);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Focus");
          lcd.setCursor(0,1);
          lcd.print("Press A to exit");
          focusFlag = 2;

        //Focus button pressed again, enlarger unpowered
        }else{

           relayStatus(false);
           focusFlag = 1;
           lcd.clear();
           dispTimeEntry();
           showEnteredTime();
           currentState = 1;
          }
        break;

      //Clear entered time values to 0
      case 'B': 
        relayStatus(false);
        currentTimeValue[0] = 0;
        currentTimeValue[1] = 0;
        currentTimeValue[2] = 0;
        currentTimeValue[3] = 0;
        currentTimeValue[4] = 0;
        break;

      //Expose for entered time
      case 'D':
        tempVal[0] = currentTimeValue[0];
        tempVal[1] = currentTimeValue[1];
        tempVal[2] = currentTimeValue[2];
        tempVal[3] = currentTimeValue[3];
        tempVal[4] = currentTimeValue[4];
        tempVal[5] = 0;

        currentState = 2;
        break;
      
      default:
        currentTimeValue[0] = currentTimeValue[1];
        currentTimeValue[1] = currentTimeValue[2];
        currentTimeValue[2] = currentTimeValue[3];
        currentTimeValue[3] = currentTimeValue[4];
        currentTimeValue[4] = key;
//        showEnteredTime();
        break;
    }
}
}

void relayStatus(bool state) {
  if (state) {
    digitalWrite(controlPin, HIGH);
  } else {
    digitalWrite(controlPin, LOW);
  }
}

void timer() {

  while ((cdrun) == 1) {
    relayStatus(true);
    
    //Has a 10th of a second passed
    //10th second decrement
    if (timeElapsed >= millisResolution) {
      --currentTimeValue[4];
      
      //Unit second decrement
      if (currentTimeValue[4] == -1) {
        --currentTimeValue[3];
        currentTimeValue[4] = 9;
        
        //10s second decrement
        if (currentTimeValue[3] == -1) {
          --currentTimeValue[2];
          currentTimeValue[3] = 9;
          
          //Unit minute decrement
          if (currentTimeValue[2] == -1) {
            --currentTimeValue[1];
            currentTimeValue[2] = 5;
            
            //10s minute decrement
            if (currentTimeValue[1] == -1) {
              --currentTimeValue[0];
              currentTimeValue[1] = 9;
            }
          }
        }
      }

      //Displaying the countdown dimer
      Serial.print(currentTimeValue[0]);
      Serial.print(currentTimeValue[1]);
      Serial.print(":");
      Serial.print(currentTimeValue[2]);
      Serial.print(currentTimeValue[3]);
      Serial.print(".");
      Serial.println(currentTimeValue[4]);

      //Reset the elapsed time counter
      timeElapsed = 0;

      //Check if countdown has finished
      if (
      (currentTimeValue[0] == 0) && 
      (currentTimeValue[1] == 0) &&
      (currentTimeValue[2] == 0) &&
      (currentTimeValue[3] == 0) &&
      (currentTimeValue[4] == 0) ) {
        stop();
      }
    }
  }
}

void showEnteredTime() {
  lcd.setCursor(0, 1);
  lcd.print(currentTimeValue[0]);
  lcd.print(currentTimeValue[1]);
  lcd.print(":");
  lcd.print(currentTimeValue[2]);
  lcd.print(currentTimeValue[3]);
  lcd.print(".");
  lcd.print(currentTimeValue[4]);
}

void stop() {
  Serial.println("Countdown finished");
  relayStatus(false);
  cdrun = 0;
}

void dispTimeEntry() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Time mm:ss:");
}
