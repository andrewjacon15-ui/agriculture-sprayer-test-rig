# Agriculture Sprayer Test Rig -- Linear Speed Carriage

A test fixture that drives a water sprayer head along a 6 ft measured
travel zone at a controlled, repeatable speed (0.5 / 1.5 / 3.0 mph presets,
±10% tolerance), so spray behavior can be evaluated at realistic travel
speeds on the bench instead of on a moving vehicle.

Belt-driven carriage on aluminum V-slot rail, brushed DC gearmotor,
Arduino Uno controller, limit switches for end-of-travel detection
(optional encoder for closed-loop speed accuracy). Total parts cost is
roughly $455 against a $1,500 budget -- see the BOM for the breakdown.

## Where to start

| If you want to... | Read |
|---|---|
| Understand *why* the rig is designed this way | [`docs/01_System_Overview.md`](docs/01_System_Overview.md) |
| Order parts | [`docs/02_Bill_of_Materials.md`](docs/02_Bill_of_Materials.md) |
| Build it | [`docs/03_Assembly_Instructions.md`](docs/03_Assembly_Instructions.md) |
| Wire it | [`docs/04_Wiring_Electrical.md`](docs/04_Wiring_Electrical.md) |
| Run it day-to-day | [`docs/05_Software_Operation_Guide.md`](docs/05_Software_Operation_Guide.md) |
| Get the speed dialed in to ±10% | [`docs/06_Calibration_and_Tuning.md`](docs/06_Calibration_and_Tuning.md) |
| Fix something that's not working | [`docs/07_Troubleshooting.md`](docs/07_Troubleshooting.md) |
| Look up a specific part's specs/datasheet | [`docs/08_Component_Reference/`](docs/08_Component_Reference/) |
| Understand the safety design (E-stop, fail-safe switches) | [`docs/09_Safety.md`](docs/09_Safety.md) |
| Install/read the Arduino firmware | [`firmware/sprayer_carriage_control/README.md`](firmware/sprayer_carriage_control/README.md) |

## Repository layout

```
docs/                              -- all project documentation (read in order 01-09)
  08_Component_Reference/          -- one page per hardware part: specs, pinout, how to test/replace it
firmware/
  sprayer_carriage_control/
    sprayer_carriage_control.ino   -- the control logic (heavily commented)
    config.h                       -- every tunable number: pins, speeds, distances, gains
    README.md                      -- how to install the Arduino IDE and upload
```

## Design at a glance

```
|<--- 12 in --->|<-------------- 72 in (6 ft) -------------->|<--- 12 in --->|
   ACCELERATE                    CRUISE                          DECELERATE
[HOME switch]                (measured zone)                  [FAR switch]
```

The motor accelerates through the first buffer, holds the selected target
speed across the measured 6 ft zone (with optional encoder feedback
actively correcting speed in real time), decelerates through the last
buffer, and stops on the far limit switch. Then it pauses, reverses, and
repeats -- looping automatically until stopped. Full reasoning in
`docs/01_System_Overview.md`.

## Safety

A hardware E-stop cuts motor power independent of the firmware, and both
limit switches are wired so a broken wire fails the same way as actually
reaching the end of travel (motor stops). See `docs/09_Safety.md` before
first power-up.

## Questions during your build

If something in the design doesn't fit your specific setup (a different
motor, a different rail length, a different budget target), the reasoning
behind every number is documented in `docs/01_System_Overview.md` and
`docs/02_Bill_of_Materials.md` specifically so you can adapt it rather than
needing to treat it as fixed.
