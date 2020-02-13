#include <elapsedMillis.h>

elapsedMillis timeElapsed;
unsigned int millisResolution = 100;
unsigned int tick = 100;
int timerMins = 0;
int timerSec = 9;
int timer10th = 9;

void setup() {
  Serial.begin(9600);

}

void loop() {
  char timerDisp[6];

  if (timeElapsed >= millisResolution){
    --timer10th;
    if (timer10th <= -1){
      --timerSec;
      timer10th = 9;
      if (timerSec <= 0){
        --timerMins;
        timerSec = 59;
      }
    }
    sprintf(timerDisp, "%.2d:%.2d.%.1d", timerMins, timerSec, timer10th);
    Serial.println(timerDisp);
    tick = millisResolution;
    timeElapsed = 0;

    if (timerMins or timerSec & timer10th == -1){
      stop();
    }
  }
}

void stop() {
  Serial.println("Countdown finished");
}
