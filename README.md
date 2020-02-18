
# BM/LWTimer

A simple Arduino controlled, programmable linear countdown timer switch for photographic enlargers.
It is currently capable of manual override (i.e for focusing the image), pausing mid-exposure and picking up at the same point in the countdown & automatically remembering the last time entered for repeat exposures.

The timers range is 99mins 59sec 9/10sec - 1/10th of a second.
The required time is entered via a number pad with a 4th column for function buttons. 
There is no need to enter preceeding zeros when entering the time; for example to enter 30.5seconds, one would press "3", "0" and "5". However, if you do not require sub-second resolution, you still need to enter the last zero. i.e. 5seconds would be "5" & "0". Just entering "5" would result in a half second exposure.

### Function buttons:
- **A:** - In the idle state this button powers the enlarger for **focusing** and set-up. There is no function whilst in the exposure state.
- **B:** - In the idle state this button **zeros** the counter. In the exposure state in **pauses** the counter and turns off the enlarger.
- **C:** - No function currently.
- **D:** - In the idle state it will **run** the programmed exposure. There is no function in the exposure state
- **\*:** - No function currently.
- **\#:** - No function currently.

### Hardware:
- Arduino nano
- 16x2 IIC LCD screen Red on black (or covered with rubylith or similar filter)
- 4x4 keypad
- relay capable of handling input power of your enlarger (I used a 40A SSR)

### Libraries:
- wire.h
- liquidcrystal_i2c.h
- keypad.h
- elapsedMillis.h

### Known issues:
- Currently if you press **D** while the counter is zero'd, it will start to run the counter from -20:00.0 whilst powering the enlarger.
