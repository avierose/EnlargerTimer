//  BonsaiMad Enlarger Timer project by A Wesley-August 14.08.2019, updated 18.2.2020
//  Code is open source and free to be modified for non-profit purposes
//  Check out leigh.works & bonsaimad.co.uk
//
//  Hardware:
//    *Arduino Nano
//    *4x4 Keypad
//        1 -> 12
//        2 -> 11
//        3 -> 10
//        4 -> 9
//        5 -> 8
//        6 -> 7
//        7 -> 6
//        8 -> 5
//    *16x2 IIC LCD (preferrably red on black, but can use safelight such as 'Kodak 1A' filter over other colours)
//        VCC -> 5v
//        GND -> GND
//        SDA -> A4
//        SCL -> A5
//        LED+ -> 3
//    *Relay
//       Signal -> pin 4
//
// -------------------------------------------------ISSUES TO ADDRESS------------------------------------------
//      If entered time is at 0 when pressing 'D', timer starts counting down from -19:00.0
// ------------------------------------------------------------------------------------------------------------


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <elapsedMillis.h>

elapsedMillis timeElapsed;
unsigned int millisResolution = 100;

int controlPin = 4;
int backlightPin = 3;
int backlightVal = 60;
int currentTimeValue[5];
int memVal[5];
int currentState = 1;
int focusFlag = 1;
int cdrun = 0;
String versionNumber = "V1.2.alpha";

//define the keypad
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {

  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[rows] = {5, 6, 7, 8};
byte colPins[cols] = {9, 10, 11, 12};

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
  relayStatus(false);
  Serial.begin(9600);
  //setup and turn off relay
  pinMode(controlPin, OUTPUT);


  //Initialize the lcd
  lcd.init();

  //Welcome Screen.
  lcd.backlight();
  analogWrite(backlightPin, backlightVal);

  lcd.clear();
  lcd.createChar(0, ghosty);
  lcd.setCursor(0, 0);
  lcd.print(" BonsaiMadTimer ");
  lcd.setCursor(0, 1);
  lcd.print(char(0));
  lcd.print(" " + versionNumber + " ");
  lcd.print(char(0));

  delay(2000);
//
//  lcd.clear();
//  lcd.setCursor(0, 0);
//  lcd.print(char(0));
//  lcd.print(" Leigh Works ");
//  lcd.print(char(0));
//  lcd.setCursor(0, 1);
//  lcd.print("Durst M370");
//
//  delay(2000);

  //display main screen
  lcd.clear();
  dispTimeEntry();

  //setup default time to 00:00.0
  currentTimeValue[0] = '0' - 48; // " - 48 " is converting the char value into the integer value
  currentTimeValue[1] = '0' - 48;
  currentTimeValue[2] = '0' - 48;
  currentTimeValue[3] = '0' - 48;
  currentTimeValue[4] = '0' - 48;
  showEnteredTime(false);
}

void loop() {
  char key = keypad.getKey();

  //key pressed and currentState is 1
  if (int(key) != NO_KEY and currentState == 1) {

    switch (key) {
      // Manual override of the relay when not counting down for focus purposes
      case 'A':

        //Focus button pressed, englarger powered
        if ((focusFlag) == 1) {

          relayStatus(true);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Focus");
          lcd.setCursor(0, 1);
          lcd.print("Press A to exit");
          focusFlag = 2;

          //Focus button pressed again, enlarger unpowered
        } else {

          relayStatus(false);
          focusFlag = 1;
          lcd.clear();
          dispTimeEntry();
          showEnteredTime(false);
          currentState = 1;
        }
        break;

      //Clear entered time values to 0
      case 'B':
        relayStatus(false);
        currentTimeValue[0] = '0' - 48;  // - 48 " is converting the char value into the integer value
        currentTimeValue[1] = '0' - 48;
        currentTimeValue[2] = '0' - 48;
        currentTimeValue[3] = '0' - 48;
        currentTimeValue[4] = '0' - 48;
        showEnteredTime(false);
        currentState = 1;
        break;

      //Currently not used
      case 'C':
        break;

      //Expose for entered time
      case 'D': 
        //Commit the entered time into a memory array
        memVal[0] = currentTimeValue[0];
        memVal[1] = currentTimeValue[1];
        memVal[2] = currentTimeValue[2];
        memVal[3] = currentTimeValue[3];
        memVal[4] = currentTimeValue[4];

        //Change currentState & set timer to run
        currentState = 2;
        cdrun = 1;
        break;

      //Currently not used
      case '*':
        break;

      //Currently not used
      case '#':
        break;

      default:
        //Successive keystokes move the values up a digit position
        currentTimeValue[0] = currentTimeValue[1];
        currentTimeValue[1] = currentTimeValue[2];
        currentTimeValue[2] = currentTimeValue[3];
        currentTimeValue[3] = currentTimeValue[4];
        currentTimeValue[4] = key - 48; // " - 48 " is converting the char value into the integer value
        showEnteredTime(false);
        break;
    }
  }

  if (currentState == 2) {
    if (int(key) != NO_KEY) {

      // Cancel the exposure and return to time entry screen, original time value remains
      if (key == 'B') {
        relayStatus(false);
        lcd.clear();
        dispTimeEntry();
        showEnteredTime(false);
        currentState = 1;
        cdrun = 0;
      }

    } else {

      //Counting down
      while ((cdrun) == 1) {
        relayStatus(true);

        //Has a 1^-10 of a second passed
        // 1/10th second decrement
        if (timeElapsed >= millisResolution) {
          --currentTimeValue[4];

          // Second decrement
          if (currentTimeValue[4] == -1) {
            --currentTimeValue[3];
            currentTimeValue[4] = 9;

            // 1^10 second decrement
            if (currentTimeValue[3] == -1) {
              --currentTimeValue[2];
              currentTimeValue[3] = 9;

              // Minute decrement
              if (currentTimeValue[2] == -1) {
                --currentTimeValue[1];
                currentTimeValue[2] = 5;

                // 1^10 Minute decrement
                if (currentTimeValue[1] == -1) {
                  --currentTimeValue[0];
                  currentTimeValue[1] = 9;
                }
              }
            }
          }

          //Displaying the countdown timer
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("EXPOSING");
          lcd.setCursor(9, 1);
          lcd.print(currentTimeValue[0]);
          lcd.print(currentTimeValue[1]);
          lcd.print(":");
          lcd.print(currentTimeValue[2]);
          lcd.print(currentTimeValue[3]);
          lcd.print(".");
          lcd.println(currentTimeValue[4]);

          //Reset the elapsed time counter
          timeElapsed = 0;

          //Check if countdown has finished
          if (
            (currentTimeValue[0] == 0) &&
            (currentTimeValue[1] == 0) &&
            (currentTimeValue[2] == 0) &&
            (currentTimeValue[3] == 0) &&
            (currentTimeValue[4] == 0) ) {
            currentState = 1;
            cdrun = 0;
            relayStatus(false);
            lcd.clear();
            dispTimeEntry();
            showEnteredTime(true); //Passing a true state so that the stored time is recalled

          } else {

            relayStatus(true);
          }
          break;
        }
      }
    }
  }
}

//Displaying the time entered
void showEnteredTime(bool cdEnd) {
  
  if (cdEnd){ // If boolean state is true, recall stored value from memVal[]
  currentTimeValue[0] = memVal[0];
  currentTimeValue[1] = memVal[1];
  currentTimeValue[2] = memVal[2];
  currentTimeValue[3] = memVal[3];
  currentTimeValue[4] = memVal[4];
  }
  
  lcd.setCursor(9, 1);
  lcd.print(currentTimeValue[0]);
  lcd.print(currentTimeValue[1]);
  lcd.print(":");
  lcd.print(currentTimeValue[2]);
  lcd.print(currentTimeValue[3]);
  lcd.print(".");
  lcd.print(currentTimeValue[4]);
}

//Controling the output to the relay & backlight dependent on the state the timer is in
void relayStatus(bool state) {
  if (state) {
    digitalWrite(controlPin, HIGH);
    analogWrite(backlightPin, 10);
  } else {
    digitalWrite(controlPin, LOW);
    analogWrite(backlightPin, backlightVal);
  }
}

//Displaying user prompt
void dispTimeEntry() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ent Time MM:SS.s");
}
