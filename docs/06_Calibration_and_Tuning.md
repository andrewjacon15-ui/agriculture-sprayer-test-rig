# 06 -- Calibration & Tuning

The numbers in `config.h` are calculated from the motor and pulley's
published specs, but "published specs" and "your actual motor under your
actual load" are never identical -- friction, exact supply voltage, and
manufacturing tolerance all shift things a bit. This doc is how you close
that gap and confirm you're actually hitting ±10%.

## You need a stopwatch either way

Even with the encoder installed, calibrate against a stopwatch at least
once, so you have an independent check that the encoder math (gear ratio,
pulley circumference, counts-per-inch) was entered correctly in `config.h`.
A phone's stopwatch app is accurate enough for this.

## Step 1 -- Mark the measured zone

On the rail itself (tape or a paint marker), mark the true start and end of
the 6 ft measured zone: 12 in from the HOME switch, and 12 in before the
FAR switch. You'll time the carriage crossing between these two marks, not
the whole rail.

## Step 2 -- Time each speed setting

For each of LOW, NOMINAL, and MAX:

1. Select that mode and press START.
2. Time (with the stopwatch) how long the carriage takes to cross between
   your two marks, in the direction you're watching. Do this for **at
   least 3 passes** in each direction and average them -- a single pass can
   be thrown off by your own reaction time.
3. Compute actual speed: `speed_mph = (6 ft / avg_seconds) × 0.6818`. (That
   conversion factor turns ft/s into mph.)
4. Compare to the target (0.5 / 1.5 / 3.0 mph). If it's outside ±10%,
   continue to Step 3 below.

## Step 3a -- If you built WITHOUT the encoder (open-loop)

Adjust the `PWM_LOW` / `PWM_NOMINAL` / `PWM_MAX` values in `config.h`
directly:

- Running too slow? Raise that PWM value.
- Running too fast? Lower it.
- PWM is 0-255. A useful rule of thumb: a 5% change in PWM value produces
  roughly a 5% change in speed (the relationship is close to linear for a
  DC motor under a light, steady load like this one) -- so if you measured
  9% slow, try raising the PWM value by about 9%, re-upload, and re-time.
- If MAX speed won't reach target even at PWM=255, your supply voltage or
  drivetrain has more loss than assumed -- see `07_Troubleshooting.md`
  ("Can't reach target speed even at full PWM").
- Re-run Step 2 after every change until all three modes read within ±10%
  in both directions.

## Step 3b -- If you built WITH the encoder (closed-loop)

The firmware is already correcting speed in real time during CRUISE, so if
you're still outside tolerance, something is wrong with the correction
itself, not just a fixed offset. Check in this order:

1. **Confirm the geometry constants are right first.** In `config.h`,
   double check `GEARBOX_RATIO` (19.0 for the BOM's motor), `ENCODER_CPR`
   (64.0), and `PULLEY_CIRCUMFERENCE_IN` (6.2992 for the 80T pulley in the
   BOM). If you substituted a different motor or pulley, these MUST match
   your actual parts or every downstream calculation is wrong no matter how
   well you tune the control gains.
2. **Watch `measuredIn/s` in the telemetry** during a CRUISE phase. Compare
   it to your stopwatch-measured value from Step 2. If they disagree with
   each other by more than a percent or two, the geometry constants (not
   the control loop) are the problem -- fix those first.
3. **If `measuredIn/s` matches your stopwatch but still isn't near
   `targetIn/s`,** or if the PWM value visibly hunts up and down without
   settling, tune `SPEED_KP` and `SPEED_KI` in `config.h`:
   - Speed oscillates / motor "surges" rhythmically: **lower `SPEED_KP`**
     by about 30% and re-test.
   - Speed responds sluggishly, taking most of the 6 ft zone to settle near
     target: **raise `SPEED_KP`** by about 30% and re-test.
   - Speed gets close but settles a little off target and stays there:
     **raise `SPEED_KI`** slightly (small steps -- this term accumulates
     over time and too much causes slow oscillation).
   - Change one gain at a time, re-upload, re-run a pass, and watch the
     telemetry before changing the other.

## Step 4 -- Confirm the buffer distance is still adequate

If you changed `RAMP_TIME_MS` or significantly changed the top speed while
tuning, re-check the math in `01_System_Overview.md` ("Why 12 inches of
buffer specifically") to make sure the accelerate/decelerate distance still
fits inside `BUFFER_IN`. The firmware will raise a FAULT if a limit switch
trips earlier than expected, which is exactly this check happening live --
but it's better to confirm it on paper first.

## Step 5 -- Record your final numbers

Once everything reads within tolerance, write your final `config.h` values
(or just keep the file) somewhere safe, and note the actual measured speeds
next to the targets. That log is useful both as a calibration record and as
a first troubleshooting reference if the rig's behavior ever drifts later
(worn belt, different battery, etc.) -- you'll have a "known good" baseline
to compare against.
