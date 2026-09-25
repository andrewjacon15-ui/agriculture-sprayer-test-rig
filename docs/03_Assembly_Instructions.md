# 03 -- Assembly Instructions

Read `01_System_Overview.md` first if you haven't -- it explains *why* the
rig is laid out this way, which makes these steps easier to follow (and
easier to adapt if your parts differ slightly from the BOM).

Tools you'll need: 2.5mm and 4mm hex (Allen) keys (usually included with
V-slot hardware kits), a Phillips screwdriver, a drill (for the motor
bracket and switch brackets), a hacksaw or angle grinder (to cut the
aluminum bar stock for the motor mount, if it doesn't come pre-cut), a
soldering iron (a few connections), wire strippers, and a multimeter.

## Step 1 -- Lay out the rail

1. Place the 96 in V-Slot 20x40 rail on a flat, stable surface (a workbench,
   or the frame/boom you're mounting it to).
2. Mark the rail at **12 in** and **84 in** from one end (call that end
   "HOME"). These marks are where the measured 6 ft zone starts and ends --
   they're just reference marks for your own sanity checking later, not a
   mounting point.
3. Secure the rail to its supporting structure. Because it's an 8 ft
   unsupported span, support it at intervals no greater than 24 in if
   possible (e.g., bolt it down every 2 ft along a boom or frame) to avoid
   sag, which would add friction and inconsistent speed.

## Step 2 -- Build the carriage

1. Assemble the OpenBuilds V-Slot gantry kit per its included instructions:
   press the eccentric spacers and POM wheels onto the gantry plate's
   mounting bolts, then thread the assembly onto the rail's V-groove.
2. Adjust the eccentric spacers (rotate them slightly with a wrench) until
   the plate rolls smoothly along the rail with no side-to-side wobble and
   no binding. This is the single most important adjustment for consistent
   speed -- too tight and the motor fights friction, too loose and the
   carriage wobbles. Roll it the full length of the rail by hand and
   confirm it's smooth everywhere before moving on.
3. This gantry plate is where you'll mount your sprayer head hardware
   (nozzle, water line clip, etc.) -- that part is specific to your sprayer
   assembly and outside the scope of this rig.

## Step 3 -- Build and mount the motor bracket

The Pololu 37D gearmotor doesn't have an off-the-shelf V-slot mount, so this
is the one part you fabricate:

1. Cut a piece of the 1/8 x 1 in aluminum flat bar to ~4 in long.
2. Bend it into an L-shape (or use two pieces + a corner joint) so one leg
   bolts flat to the end of the V-slot rail (using an M5 T-nut) and the
   other leg holds the motor.
3. Drill two holes in the motor-facing leg matching the Pololu 37D's
   mounting-hole pattern (the two threaded holes on the gearbox face -- see
   `08_Component_Reference/Pololu_37D_Gearmotor.md` for exact dimensions)
   and a center clearance hole for the output shaft.
4. Bolt the motor to the bracket, then bolt the bracket to the HOME end of
   the rail (position it so the pulley on the motor shaft lines up with the
   belt's centerline over the carriage).

## Step 4 -- Mount the idler pulley

1. At the FAR end of the rail, mount the shoulder-screw idler pulley to a
   small bracket (same approach as the motor bracket, simpler since it's
   just a pivot point) so it's aligned with the drive pulley at the other
   end.
2. The idler pulley just needs to spin freely on its shoulder screw --
   snug the screw enough that it doesn't wobble, but loose enough that the
   pulley turns freely by hand.

## Step 5 -- Install the belt

1. Cut the GT2 belt to length: `2 × 96 in + ~8 in slack ≈ 200 in`. Better to
   cut long and trim than come up short.
2. Route the belt around the drive pulley (motor end) and idler pulley (far
   end), forming a loop the full length of the rail.
3. Clamp both ends of the belt to the carriage plate (a small aluminum bar
   + two screws pressing the belt against the plate works well -- this is
   the standard "belt clamp" approach used on 3D printers; search "GT2 belt
   clamp carriage" for reference photos if you want a visual).
4. Tension the belt: it should be firm enough that it doesn't sag or skip
   teeth on the pulleys, but not so tight that it strains the idler bracket.
   Pluck it like a guitar string -- a dull thud means too loose, a taut
   twang means about right.
5. Roll the carriage by hand across the full rail length and confirm the
   belt tracks straight on both pulleys without rubbing the pulley flanges.

## Step 6 -- Mount the limit switches

1. Mount one Omron V-156 switch near the **HOME** end (0 in mark) and one
   near the **FAR** end (96 in mark), positioned so the carriage's roller
   or a small tab on the carriage plate physically depresses the switch
   lever a few millimeters before the carriage would otherwise run out of
   rail or crash into the pulley bracket.
2. Adjust each switch's position so it trips with roughly the same amount
   of carriage travel remaining on both ends (a few mm of pre-travel is
   fine and expected -- roller-lever switches need a little travel to fully
   actuate).
3. Confirm by hand: roll the carriage slowly into each switch and listen/
   feel for the click. It should trip *before* the carriage reaches a hard
   mechanical stop, not after.

## Step 7 -- Wire the electronics

Follow `04_Wiring_Electrical.md` for the full wiring diagram and pinout.
Summary of what gets connected:

1. Mean Well power supply -> **E-stop button (NC contact)** -> Cytron MD13S
   `VM`/`GND` terminals. The E-stop sits directly in this line, in
   hardware, before anything else.
2. Cytron MD13S `M1A`/`M1B` -> motor power leads.
3. Cytron MD13S `PWM`/`DIR`/`GND` -> Arduino `D9`/`D8`/`GND`.
4. Motor encoder leads -> Arduino `D2`/`D4`/`5V`/`GND` (if installed).
5. Both limit switches' **NC** and **COM** contacts -> Arduino `D6`/`D7`
   (with the switch's other side to `GND`).
6. MODE and START buttons -> Arduino `D11`/`D12` (other side to `GND`).
7. Mode LEDs (through 220Ω resistors) -> Arduino `A0`/`A1`/`A2`.
8. Arduino powered separately via USB (from a laptop) or a standalone 5V
   wall adapter into its barrel jack -- **not** from the 12V motor supply --
   so the Arduino stays alive and able to report status even when the
   E-stop has cut motor power.

Mount the Arduino, Cytron driver, terminal blocks, and fuse holder inside
the project enclosure. Route the motor power wires and switch signal wires
out through cable glands or grommets to the rail.

## Step 8 -- First power-on checklist

Do this **before** connecting the belt to the carriage, so an unexpected
motor spin can't fling anything:

1. With the belt disconnected from the carriage (motor free to spin), power
   up. Confirm the Arduino boots and the Serial Monitor shows the homing
   message.
2. Manually trigger each limit switch by hand and confirm the Serial
   Monitor telemetry line shows `homeSw=TRIG` / `farSw=TRIG` correctly.
3. Press START and confirm the motor spins in the expected direction at
   roughly the expected speed for the NOMINAL preset, and that pressing the
   physical E-stop immediately kills motor power (LEDs on the Arduino keep
   working since it's on its own supply).
4. Only once all of that checks out, reconnect the belt to the carriage and
   proceed to `06_Calibration_and_Tuning.md`.

## Step 9 -- Final assembly

1. Re-attach the belt to the carriage (Step 5.3).
2. Mount your sprayer head hardware to the carriage plate.
3. Run the carriage through a few slow manual passes (LOW speed mode) to
   confirm nothing binds, rubs, or catches on wiring before running at
   NOMINAL or MAX.
