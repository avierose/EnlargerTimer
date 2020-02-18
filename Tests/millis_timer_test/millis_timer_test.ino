#include <elapsedMillis.h>

elapsedMillis timeElapsed;
unsigned int millisResolution = 100;
unsigned int tick = 100;
int timerMins = 0;
int timerSec = 0;
int timer10th = 1;
int cdrun = 1;
int controlPin = 4;

void setup() {
  Serial.begin(9600);
  pinMode(controlPin, OUTPUT);
}

void loop() {
  timer();
}

void relayStatus(bool state) {
  if (state) {
    digitalWrite(controlPin, HIGH);
  } else {
    digitalWrite(controlPin, LOW);
  }
}

void timer() {
  char timerDisp[6];
  while ((cdrun) == 1) {
    relayStatus(true);
    if (timeElapsed >= millisResolution) {
      --timer10th;
      if (timer10th <= -1) {
        --timerSec;
        timer10th = 9;
        if (timerSec <= -1) {
          --timerMins;
          timerSec = 59;
        }
      }
      sprintf(timerDisp, "%.2d:%.2d.%.1d", timerMins, timerSec, timer10th);
      Serial.println(timerDisp);
      tick = millisResolution;
      timeElapsed = 0;

      if (timerMins & timerSec or timer10th == -1) {
        stop();

      }
    }
  }
}

void stop() {
  Serial.println("Countdown finished");
  relayStatus(false);
  cdrun = 0;
}
