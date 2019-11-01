
# BM/LW12VTimer

A simple Arduino controlled, programmable linear countdown timer switch for photographic enlargers. Designed to run 12V heads using DC rather than AC.

## Case study:
A Durst M370 colour head, which uses a 12vAC MR11/GU3.5 halogen bulb.

By replacing the standard supplied mains to 12vAC stepdown transformer with an inexpensive AC to DC 12v 8.5w PSU, allows for the use of the BM/LW12VTIMER without any further modification. 

Placing the timer between the PSU & the enlarger, results in the timer stepping down the supplied 12vDC to 6-7v to power the internal circuitry whilst outputting 12v to the enlarger upon operation. Since halogen bulbs will operate on AC & DC currents, no modifications need to be made to the enlarger set up.

### Hardware:
- Arduino nano
- 16x2 IIC LCD screen Red on black (or covered with rubylith or similar filter)
- 4x4 keypad
- relay capable of handling input power of your enlarger

### Libraries
- wire.h
- liquidcrystal_i2c.h
- keypad.h

