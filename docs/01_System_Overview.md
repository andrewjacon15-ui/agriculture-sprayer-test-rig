# 01 -- System Overview & Design Reasoning

## What this rig is for

A test fixture that carries a water sprayer head along a straight 6 ft path
at a controlled, repeatable speed, so the sprayer's spray pattern/output can
be evaluated at realistic travel speeds without needing a full vehicle. It's
a test tool, not a production machine -- the design favors low cost,
off-the-shelf parts, and easy troubleshooting over precision or ruggedness.

## Requirements (as given)

| Requirement | Value |
|---|---|
| Measured travel distance | 6 ft (72 in) |
| Speed tolerance | ±10% |
| Desired operating speed | 1.5 mph |
| Rated max speed | 3.0 mph |
| Adjustable lower speed | Yes (toggle-able) |
| End-of-travel sensing | Limit switches (encoder optional) |
| Motor type | Brushed or brushless DC |
| Controller | Simple Arduino |
| Budget | Under $1,500 total |
| Use case | Testing / low duty cycle, not production |

## The core design decision: belt-driven carriage on aluminum extrusion rail

We're using a linear rail made from V-slot aluminum extrusion (the same
system 3D printers and small CNC machines use), a rolling carriage plate,
and a GT2 toothed-belt drive turned by a DC gearmotor. Reasons:

- **It's a solved problem.** V-slot/GT2 systems are extremely well
  documented in the maker community (OpenBuilds, 3D printer projects), the
  parts are cheap, and assembly doesn't require machining.
- **Speed, not force, is what matters here.** The sprayer head and carriage
  are light. A belt drive trades mechanical advantage for speed, which is
  exactly the tradeoff we want (versus, say, a lead screw, which is great
  for precision/force but comparatively slow and expensive per foot).
- **Brushed DC gearmotor, not stepper/brushless/servo.** For an open-loop or
  simple-PID speed task (not position-critical), a brushed DC gearmotor
  driven by a basic H-bridge is the cheapest, simplest, most beginner
  -friendly way to get controllable speed and enough torque. Steppers are
  built for precise positioning, not raw speed, and need more complex
  drivers. This design's motor also happens to be available in an
  encoder-equipped version, satisfying "encoder optional" by making it a
  $ few upgrade rather than a different motor entirely.

## Why the rail is 8 ft long when the requirement is 6 ft

The requirement is 6 ft of *measured* travel -- the zone where speed has to
be within tolerance. A real DC motor can't be at full commanded speed the
instant power is applied; it needs some distance to accelerate. If the
motor were still speeding up while inside the 6 ft zone, the speed reading
over that zone wouldn't be the clean, constant number you want for testing.

So the rig adds a "runway" on each end:

```
|<--- 12 in --->|<-------------- 72 in (6 ft) -------------->|<--- 12 in --->|
   ACCELERATE                    CRUISE                          DECELERATE
[HOME switch]                (measured zone)                  [FAR switch]
     0 in            12 in                          84 in         96 in
```

Total rail length: **96 in (8 ft)**.

### Why 12 inches of buffer specifically

This is the "defer to your judgment" call from the brief. The math that
justifies it:

- Drivetrain: Pololu 19:1 metal gearmotor (500 RPM free-run @ 12V) turning
  an 80-tooth GT2 pulley (2 mm pitch, 6.30 in circumference) gives a
  theoretical top speed of **~2.98 mph** at full duty cycle -- landing
  almost exactly on the 3.0 mph max-speed requirement (see
  `02_Bill_of_Materials.md` for how this pulley/motor pair was chosen).
- The firmware ramps PWM linearly from 0 to target over `RAMP_TIME_MS`
  (default 350 ms, in `config.h`). For a linear ramp, the average speed
  during the ramp is half the target speed, so:

  `accel_distance ≈ 0.5 × target_speed × ramp_time`

  At worst case (3 mph = 52.8 in/s, 350 ms ramp): `0.5 × 52.8 × 0.35 ≈ 9.2
  in` -- comfortably inside a 12 in buffer, leaving ~2.8 in of margin for
  the limit switch's own pre-travel and mounting tolerance.
- At the 1.5 mph desired speed, the same math gives only ~4.6 in of ramp
  distance -- even more margin.

If you build this with a different motor/pulley combination, re-run this
calculation (or just lengthen `RAMP_TIME_MS`/shorten it) and confirm the
result stays under `BUFFER_IN` in `config.h` -- the firmware will raise a
`FAULT` and tell you if a limit switch trips earlier than expected, which is
exactly this problem showing up at runtime instead of on paper.

## Why speed is controlled the way it is

The gearmotor is available with an integrated quadrature encoder for a
small added cost, so the default build includes it and runs **closed-loop**:
the firmware counts actual shaft rotation and continuously trims the PWM
duty cycle to hold the target speed, correcting for battery sag, friction,
and belt tension automatically. This is the most reliable way to guarantee
the ±10% tolerance across a whole test session.

If you build without the encoder, the firmware falls back to **open-loop**
control: a fixed, pre-calculated PWM value per speed setting. This works,
but you're relying on the motor's speed-vs-voltage relationship staying
put, so you calibrate it by hand with a stopwatch (see
`06_Calibration_and_Tuning.md`) and should recheck it if you change supply
voltage, load, or motor.

## Why limit switches instead of relying on the encoder for position

The brief asks for limit switches as the primary end-of-travel method, with
the encoder as optional. This is a good default even *with* an encoder
installed: an encoder only measures rotation, not the carriage's true
physical position, so if the belt slips even once, encoder-based position
tracking silently drifts. A physical switch at each end is unambiguous
ground truth and fails safe (see `09_Safety.md`). This rig always treats
the switches as the authority on "have I reached the end," and uses timing
(not encoder position) to decide when to start decelerating -- see the
firmware comments in `sprayer_carriage_control.ino` for the state machine
details.

## Speed presets

| Mode | Target speed | Notes |
|---|---|---|
| LOW | 0.5 mph | For slow-motion inspection / low-flow-rate testing |
| NOMINAL | 1.5 mph | The desired operating speed from the brief |
| MAX | 3.0 mph | Rig's rated top speed |

All three are adjustable in `config.h` if your testing needs change.
