# Arduino Uno Rev3

**Role in this rig:** the controller. Runs `sprayer_carriage_control.ino`,
reads the limit switches/encoder/buttons, and commands the motor driver.

**Manufacturer product page:** https://store.arduino.cc/products/arduino-uno-rev3/
**Manufacturer datasheet:** linked from the product page ("Datasheet" tab/
download link).

## Key specs used in this design

| Spec | Value | Why it matters here |
|---|---|---|
| Microcontroller | ATmega328P | Runs the firmware |
| Digital I/O pins | 14 (D0-D13) | This build uses D2, D4, D6-D9, D11-D13 |
| PWM-capable pins | D3, D5, D6, D9, D10, D11 | Motor PWM (`D9`) must be one of these |
| Hardware interrupt pins | D2, D3 | Encoder channel A (`D2`) must be one of these for `attachInterrupt()` |
| Analog pins (usable as digital I/O) | A0-A5 | Mode LEDs use A0-A2 as plain digital outputs |
| Logic level | 5V | Matches the Cytron driver and encoder Vcc |
| Power | USB (5V) or barrel jack (7-12V, has onboard regulator) | This build powers it independently of the 12V motor rail -- see `04_Wiring_Electrical.md` |

## Full pin map for this project

See the table in `04_Wiring_Electrical.md` -- that table is the
authoritative pin assignment and must match `config.h`'s `PIN_*` defines
exactly.

## How to test this part in isolation (bench test)

1. Disconnect everything except the USB cable.
2. Upload the classic `Blink` example sketch (File > Examples > 01.Basics >
   Blink in the Arduino IDE). If the onboard LED (next to pin 13) blinks
   once per second, the board and USB connection are healthy.
3. To test a specific input pin (a limit switch or button not responding),
   upload a minimal sketch that just does
   `pinMode(PIN, INPUT_PULLUP); Serial.println(digitalRead(PIN));` in a
   loop and watch the Serial Monitor while you actuate that switch/button
   by hand.

## Sourcing a replacement

Any genuine or compatible Arduino Uno R3 (or R4) works -- this is one of
the most widely available electronics boards in the world. If using an R4
or a third-party "Uno-compatible" clone, double-check that pins D2/D3 still
support hardware interrupts and that PWM pins match, since some
clones/alternate boards remap these.
