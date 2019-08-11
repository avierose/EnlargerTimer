//Base code altered from https://bit.ly/2YWi3h1
//Leigh Works Enlarger Linear Timer project by A Wesley-August 2019
//Code is open source and free to be modified for non-profit purposes

//Hardware:
//  *Arduino Nano
//  *4x4 Keypad
//  *16x2 IIC LCD (preferrably red on black, but can use safelight filter over other colours)
//    VCC -> 5v
//    GND -> GND
//    SDA -> A4
//    SCL -> A5
//  *BRB (Big red Button)
//  #Relay
//    Signal -> pin 13


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

//constants for Control Pin

int controlPin = 13;
char currentTimeValue[4];
int currentState = 1;
int timerSeconds = 0;
int lpcnt = 0;

//define the keypad
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {

  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[rows] = {11, 10, 9, 8};
byte colPins[cols] = {7, 6, 5, 4};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  lcd.init(); // initialize the lcd

  // Print a message to the LCD.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Leigh Works");
  lcd.setCursor(0, 1);
  lcd.print("DurstM370 Colour");

  delay(2000);

  //display main screen
  lcd.clear();
  displayCodeEntryScreen();

  //setup and turn off relay
  pinMode(controlPin, OUTPUT);
  digitalWrite(controlPin, LOW);

  //setup default time to 00:00
  currentTimeValue[0] = '0';
  currentTimeValue[1] = '0';
  currentTimeValue[2] = '0';
  currentTimeValue[3] = '0';
  showEnteredTime();
}

void loop() {
  int l;
  char tempVal[3];
  char key = keypad.getKey();

  //key pressed and state is 1
  if (int(key) != 0 and currentState == 1) {

    switch (key) {
      case 'A': // Manual override of the relay when not counting down for focus purposes
        if (!relayStatus){
          relayStatus(true);
        }else {
          relayStatus(false);
        }
        
      case 'B': // Clear entered time values to 0
        relayStatus(false);
        currentTimeValue[0] = '0';
        currentTimeValue[1] = '0';
        currentTimeValue[2] = '0';
        currentTimeValue[3] = '0';
        showEnteredTime();
        currentState = 1;

        lpcnt = 0;
        timerSeconds = 0;
        break;

      case 'D':
        tempVal[0] = currentTimeValue[0];
        tempVal[1] = currentTimeValue[1];
        tempVal[2] = 0;

        timerSeconds = atol(tempVal) * 60;

        tempVal[0] = currentTimeValue[2];
        tempVal[1] = currentTimeValue[3];
        tempVal[2] = 0;

        timerSeconds = timerSeconds + atol(tempVal);
        currentState = 2;
        break;

      default:
        currentTimeValue[0] = currentTimeValue[1];
        currentTimeValue[1] = currentTimeValue[2];
        currentTimeValue[2] = currentTimeValue[3];
        currentTimeValue[3] = key;
        showEnteredTime();
        break;
    }
  }

  if (currentState == 2) {
    if (int(key) != 0) {
      if (key == '*') {
        relayStatus(false);
        displayCodeEntryScreen();
        currentTimeValue[0] = '0';
        currentTimeValue[1] = '0';
        currentTimeValue[2] = '0';
        currentTimeValue[3] = '0';
        showEnteredTime();
        currentState = 1;
        lpcnt = 0;
        timerSeconds = 0;
      }
    } else {

      if (lpcnt > 9) {
        lpcnt = 0;
        --timerSeconds;
        showCountdown();

        if (timerSeconds <= 0) {
          currentState = 1;
          relayStatus(false);
          displayCodeEntryScreen();
          showEnteredTime();
        } else {
          relayStatus(true);
        }
      }

      ++lpcnt;
      delay(100);
    }
  }
}

void showEnteredTime() {
  lcd.setCursor(11, 1);
  lcd.print(currentTimeValue[0]);
  lcd.print(currentTimeValue[1]);
  lcd.print(":");
  lcd.print(currentTimeValue[2]);
  lcd.print(currentTimeValue[3]);
}

void relayStatus(bool state) {
  if (state)
    digitalWrite(controlPin, HIGH);
  else
    digitalWrite(controlPin, LOW);
}

void showCountdown() {
  char timest[6]; \

  lcd.setCursor(0, 0);
  lcd.print("** COUNTING DOWN **");
  lcd.setCursor(0, 1);
  lcd.print("** ");
  sprintf(timest, "%d:%.2d", (timerSeconds / 60), (timerSeconds - ((timerSeconds / 60) * 60)));
  lcd.print(timest);
  lcd.print(" **");
}

void displayCodeEntryScreen() {
  clearScreen();
  lcd.setCursor(0, 0);
  lcd.print("Enter Time mm:ss:");
}

void clearScreen() {
  lcd.setCursor(0, 0);
  lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print(" ");
}
