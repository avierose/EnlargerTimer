#include <TM1637Display.h>
//https://github.com/avishorp/TM1637


#define CLK 2//pins definitions for TM1637 and can be changed to other ports
#define DIO 3
#define buttonPin 4
#define numOfSeconds(_time_) ((_time_ / 1000) % 60)
TM1637Display display(CLK,DIO);
unsigned long timeLimit = 60000;

int buttonState = 0;
int flag = 0;

const uint8_t OFF[] = {0, 0, 0, 0};
const uint8_t PLAY[] = {B01110011, B00111000, B01011111, B01101110};
void setup()
{
  // Set brightness
  display.setBrightness(0x0c);
  // Clear the display
  display.setSegments(OFF);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop(){
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    if (flag == 0) {
      countdown();
      flag = 1;
    }
    else if (flag == 1){
      //Figure out an interrupt
      flag = 0;
    }
  }
  delay(100);

}


void countdown() {
  // Calculate time remaining in ss format
  unsigned long timeRemaining = timeLimit - millis();

  while(timeRemaining > 0) {
      int seconds = numOfSeconds(timeRemaining);

      // Display seconds in the last two digits of display
      display.showNumberDecEx(seconds, 0, true, 2, 2);

      // Update time remaining
      timeRemaining = timeLimit - millis();
  }
}
void buttonPressed() {
    countdown();
}
