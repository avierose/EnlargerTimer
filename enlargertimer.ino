//Base code altered from https://bit.ly/2YWi3h1
//Leigh Works Enlarger Linear Timer project by A Wesley-August 14.08.2019
//Code is open source and free to be modified for non-profit purposes
//Check out leigh.works & bonsaimad.co.uk

//Hardware:
//  *Arduino Nano
//  *4x4 Keypad
//      1 -> 11
//      2 -> 10
//      3 -> 9
//      4 -> 8
//      5 -> 7
//      6 -> 5
//      7 -> 6
//      8 -> 4
//  *16x2 IIC LCD (preferrably red on black, but can use safelight such as 'Kodak 1A' filter over other colours)
//      VCC -> 5v
//      GND -> GND
//      SDA -> A4
//      SCL -> A5
//  #Relay
//      Signal -> pin 13


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

int controlPin = 13;
char currentTimeValue[4];
int currentState = 1;
int focusFlag = 1;
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

// Define Ghosty Character
byte ghosty[8] = { 
  B00000,
  B01110,
  B11111,
  B10101,
  B11111,
  B11111,
  B10101,
  B00000
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);
  //setup and turn off relay
  pinMode(controlPin, OUTPUT);
  relayStatus(false);

  //Initialize the lcd
  lcd.init(); 

  //Welcome Screen.
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, ghosty);
  lcd.setCursor(0, 0);
  lcd.print(char(0));
  lcd.print(" Leigh Works ");
  lcd.print(char(0));
  lcd.setCursor(0, 1);
  lcd.print("Durst M370");

  delay(2000);

  //display main screen
  lcd.clear();
  dispTimeEntry();

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
           lpcnt = 0;
           timerSeconds = 0;
          }
        break;

      //Clear entered time values to 0
      case 'B': 
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
        
      //Currently not used
      case 'C':  
        break;

      //Expose for entered time
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

      // Cancel the exposure and return to time entry screen, original time value remains
      if (key == 'B') { 
        relayStatus(false);
        lcd.clear();
        dispTimeEntry();
        showEnteredTime();
        currentState = 1;
        lpcnt = 0;
        timerSeconds = 0;
      }
      
    } else {

      //Counting down
      if (lpcnt > 9) {
        lpcnt = 0;
        --timerSeconds;
        showCountdown();
        
        //Countdown finished, returns to main screen
        if (timerSeconds <= 0) {
          currentState = 1;
          relayStatus(false);
          lcd.clear();
          dispTimeEntry();
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
  char timest[6];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("EXPOSING");
  lcd.setCursor(0, 1);
  sprintf(timest, "%d:%.2d", (timerSeconds / 60), (timerSeconds - ((timerSeconds / 60) * 60)));
  lcd.print(timest);
}

void dispTimeEntry() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Time mm:ss:");
}
