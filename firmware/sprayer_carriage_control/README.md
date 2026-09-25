# Sprayer Carriage Control Firmware

Two files, both in this folder:

- **`config.h`** -- every tunable number (pins, speeds, distances, control
  gains). Change the rig's behavior here.
- **`sprayer_carriage_control.ino`** -- the logic that decides *when* things
  happen. You shouldn't need to edit this unless you're changing how the rig
  behaves, not just its numbers.

## Installing the Arduino IDE and uploading

1. Download and install the free Arduino IDE from arduino.cc (search "Arduino
   IDE download" -- version 2.x is current as of 2026).
2. Open `sprayer_carriage_control.ino` in the IDE (it will automatically open
   `config.h` in a second tab -- leave it in the same folder).
3. Plug the Arduino Uno into your computer with a USB cable.
4. In the IDE: **Tools > Board > Arduino Uno**.
5. In the IDE: **Tools > Port**, pick the port that appeared when you plugged
   in the Uno (on Windows it'll be a `COMx`; on Mac/Linux `/dev/tty.usb...` or
   `/dev/ttyACM0`).
6. Click **Upload** (the right-arrow icon). No extra libraries are required
   -- this sketch only uses built-in Arduino functions.
7. Open **Tools > Serial Monitor**, set the baud rate dropdown (bottom-right
   of that window) to **115200** to match `SERIAL_BAUD` in `config.h`. You
   should see boot messages and then a homing sequence.

## What you'll see on first power-up

1. The rig immediately drives slowly toward the HOME limit switch
   ("Homing"). This is normal and happens every time the rig is powered on,
   because it has no way to know where the carriage is sitting otherwise.
2. Once it reaches the HOME switch, it stops and waits (IDLE). The NOMINAL
   speed LED should be lit (that's the default mode).
3. Press the **MODE** button to cycle LOW -> NOMINAL -> MAX -> LOW.
4. Press the **START** button to begin the automatic back-and-forth run.
   Press it again at any time to stop (the rig finishes decelerating and
   parks at whichever end it reaches next, rather than stopping abruptly
   mid-travel).

## If something goes wrong

The rig will print `*** FAULT: ...` to the Serial Monitor with a plain-English
explanation and stop everything. All three mode LEDs will blink together.
Press START to clear the fault and re-home. See
`../../docs/07_Troubleshooting.md` for what each fault message means.

## Before you trust the speed numbers

The PWM values in `config.h` (`PWM_LOW`, `PWM_NOMINAL`, `PWM_MAX`) are a
calculated starting point, not a guarantee. Walk through
`../../docs/06_Calibration_and_Tuning.md` with a stopwatch (or use the
built-in encoder telemetry, if you wired one up) before relying on this rig
to hit its ±10% speed tolerance.
