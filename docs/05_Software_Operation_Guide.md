# 05 -- Software / Day-to-Day Operation Guide

This is the "how do I actually run a test" reference. For installing the
firmware in the first place, see
`firmware/sprayer_carriage_control/README.md`. For what the code is doing
internally, the `.ino` file itself is heavily commented -- start there if
you want the mechanism, not just the button-pushing.

## Powering up

1. Turn on the Mean Well supply (12V motor power) and make sure the E-stop
   is not pressed.
2. The Arduino should already be running (it's on its own power source --
   USB or a separate 5V adapter). If you want to watch what it's doing,
   open the Arduino IDE's Serial Monitor at 115200 baud.
3. The rig automatically **homes** on every power-up: it drives slowly
   toward the HOME limit switch and stops there. This takes a few seconds.
   You'll see `Homing complete.` in the Serial Monitor and the carriage
   will be sitting at the HOME end.

## Choosing a speed

- The three mode LEDs show the current speed setting: LOW, NOMINAL, MAX.
- Press the **MODE** button to cycle through them. You can only change mode
  while the rig is idle (stopped) -- this is intentional, so a test run is
  always one consistent speed from start to finish rather than changing
  mid-run.

## Running a test pass

1. With the desired speed mode selected, press **START**.
2. The carriage accelerates through the first 12 in buffer, holds constant
   speed through the measured 6 ft zone, decelerates through the last 12 in
   buffer, and stops at the far limit switch.
3. After a short pause, it automatically reverses and repeats the process
   back toward HOME -- and keeps doing this indefinitely (LOW/NOMINAL/MAX
   back-and-forth passes) until you press **START** again to stop it.
4. Pressing **START** while a pass is in progress doesn't slam the brakes
   mid-travel -- it finishes decelerating and parks at whichever end it's
   headed toward, then returns to IDLE instead of reversing again.

## Reading the Serial Monitor telemetry

Every ~200 ms the firmware prints one line like:

```
state=CRUISE mode=NOM dir=FAR pwm=128 targetIn/s=26.40 measuredIn/s=25.91 homeSw=open farSw=open
```

| Field | Meaning |
|---|---|
| `state` | Current phase: HOMING, IDLE, ACCEL, CRUISE, DECEL, END_PAUSE, or FAULT |
| `mode` | Selected speed preset |
| `dir` | Direction of current travel |
| `pwm` | Current motor duty cycle, 0-255 |
| `targetIn/s` | Target speed for the current pass, in inches/second |
| `measuredIn/s` | Actual measured speed from the encoder (only printed if `ENCODER_ENABLED`) |
| `homeSw` / `farSw` | Live limit switch state |

This line is plain-text and comma-free on purpose so you can copy a chunk
of it straight into a spreadsheet (or pipe the Serial output to a `.txt`
file via `screen`, PuTTY, or the Arduino IDE's own logging) for a test
report, without needing any extra software.

## Emergency stop

Press the physical E-stop button. This cuts 12V motor power in hardware,
immediately, regardless of what state the firmware is in. The Arduino
stays powered and keeps printing telemetry so you can see exactly what the
rig was doing when you stopped it. To resume, release/twist-reset the
E-stop button, then power-cycle the 12V supply (or just wait for the
supply's output to return) and let the rig re-home before pressing START
again.

## Fault conditions

If the firmware detects something inconsistent with normal operation (a
limit switch tripping somewhere it shouldn't, or not tripping when
expected), it stops the motor, prints a `*** FAULT: ...` message explaining
what happened, and blinks all three mode LEDs together. Press **START** to
clear the fault and re-home. See `07_Troubleshooting.md` for what each
fault message means and how to fix the underlying cause.
