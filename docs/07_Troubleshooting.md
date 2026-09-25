# 07 -- Troubleshooting

Organized by symptom. Each entry says what's probably wrong and where to
look to confirm it.

## Firmware fault messages

### "Hit the FAR switch while homing toward HOME"

The motor is spinning the wrong direction relative to what the firmware
expects. Swap the two motor leads on the Cytron `M1A`/`M1B` terminals (do
not edit the code -- see `04_Wiring_Electrical.md`, "Motor direction").

### "Unexpected limit switch triggered during ACCEL / during CRUISE"

A limit switch tripped before the firmware expected the carriage to be
anywhere near that end. Most likely causes, in order of likelihood:

1. **A switch is mounted too far into the rail** (tripping with more than
   12 in of travel still remaining) -- re-check switch position per
   `03_Assembly_Instructions.md` Step 6.
2. **`RAMP_TIME_MS` and/or the speed setting is too aggressive for
   `BUFFER_IN`** -- re-run the math in `01_System_Overview.md` and either
   shorten `RAMP_TIME_MS` or lengthen `BUFFER_IN` (which means physically
   moving the switch further from the end, or lengthening the rail).
3. **A switch is chattering/bouncing** due to vibration or a loose mount --
   tighten its bracket.

### "Expected limit switch never triggered within 3 s of the planned stop time"

The opposite problem: the carriage is taking longer than expected to reach
the end. Check:

1. **Belt tension** -- a loose or skipping belt makes the carriage travel
   slower than the motor's rotation would predict.
2. **Carriage friction** -- roll the carriage by hand along the whole rail;
   it should move smoothly everywhere. Re-adjust the V-wheel eccentric
   spacers if it binds anywhere (`03_Assembly_Instructions.md` Step 2).
3. **Switch not actually reachable** -- confirm nothing (a cable, a bracket)
   is physically blocking the carriage from reaching the switch.
4. **Supply voltage sagging under load** -- check the 12V rail at the
   Cytron `VM` terminal with a multimeter while the motor is running; if it
   drops well below 12V, the power supply may be undersized or a connection
   has high resistance (loose terminal, thin wire gauge).

## Physical / mechanical

### Carriage doesn't move at all when I press START

1. Check the E-stop isn't engaged.
2. Check the Mean Well supply's output with a multimeter (should read
   ~12V at its terminals).
3. Check the inline fuse hasn't blown.
4. Check for 12V at the Cytron `VM`/`GND` terminals -- if it's present at
   the supply but not here, look for a loose connection or a mis-seated
   E-stop contact in between.
5. Check the Serial Monitor telemetry: is `state=IDLE` and does pressing
   START change `running` (visible as a printed "RUN requested" message)?
   If the button press isn't registering at all, check the button's wiring
   to `D12`/GND.

### Carriage moves, but very weakly / stalls under light finger pressure

Motor is likely not getting close to 12V. Check wire gauge (18 AWG for
motor power, per the BOM) and connection quality -- thin wire or a loose
screw terminal adds resistance that shows up as voltage drop exactly when
current draw increases (i.e., under load or during acceleration).

### Carriage speed is inconsistent pass-to-pass

1. If running open-loop (no encoder): this is expected to some degree,
   since nothing is correcting for friction/voltage changes in real time.
   If it's beyond ±10%, revisit `06_Calibration_and_Tuning.md`.
2. If running closed-loop (encoder installed): inconsistent speed despite
   active correction usually points to a mechanical issue the control loop
   can't fully compensate for -- binding V-wheels, a belt that's
   inconsistently tensioned, or a connector that's intermittently loose.
   Recheck assembly Step 2 (carriage) and Step 5 (belt).

### Belt skips teeth on the pulley

Belt tension is too low, or the pulley's set screw has loosened and it's
slipping on the motor shaft (not actually turning with the motor).
Re-tension per `03_Assembly_Instructions.md` Step 5, and check the set
screw is seated against the motor shaft's flat (D-shaft), not just
clamping round stock.

## Can't reach target speed even at full PWM

If MAX mode measures noticeably under 3 mph even at `PWM_MAX = 255`:

1. Confirm supply voltage under load (see above) -- a sagging supply
   directly reduces top speed.
2. Confirm the pulley is actually 80-tooth as specified, not a smaller
   stock pulley substituted during a parts swap -- a smaller pulley
   directly reduces linear speed for the same motor RPM.
3. This can also simply mean the real motor's loaded RPM is a bit under its
   published no-load RPM (normal -- no-load numbers are always optimistic).
   If you're close but not quite there, that's expected and fine as long as
   NOMINAL (1.5 mph) is well within tolerance, since that's the speed the
   brief actually calls "desired." If you need more headroom at the top
   end, consider a slightly larger pulley (e.g. 90-100 tooth) or a supply
   voltage bump within the motor's rating -- talk to whoever's driving this
   requirement before changing it, since it changes the accel-distance math
   in `01_System_Overview.md` too.

## Serial Monitor shows nothing / garbage characters

Baud rate mismatch. Set the Serial Monitor's baud dropdown to match
`SERIAL_BAUD` in `config.h` (115200 by default).

## Still stuck

Capture a chunk of Serial Monitor telemetry from just before and during the
problem (see `05_Software_Operation_Guide.md` on logging it to a text
file) -- having the actual `state=... pwm=... measuredIn/s=...` sequence is
far more useful for diagnosis than a description of the symptom alone.
