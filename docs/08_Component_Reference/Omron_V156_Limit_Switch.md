# Omron V-156-1C25 -- Roller Lever Limit Switch

**Role in this rig:** one is mounted at each end of the rail. Physically
detects when the carriage has reached that end of travel and tells the
Arduino to stop/reverse the motor. This is the rig's primary (and, without
the optional encoder, only) position-sensing method.

**Manufacturer product family page:** https://www.omron.com/ (search
"V-15" / "V-156" basic switches) -- also stocked with datasheets at
DigiKey: https://www.digikey.com/en/products/detail/omron-electronics-inc-emc-div/V-156-1C25/5237043

## Key specs used in this design

| Spec | Value | Why it matters here |
|---|---|---|
| Contact configuration | SPDT (single pole, double throw) -- COM, NO, NC terminals | We use COM + NC only, see below |
| Rated current | 15A @ 250VAC | Far more than needed for a 5V logic signal -- these are actually industrial-duty switches, oversized for this job on purpose because they're cheap, robust, and widely stocked |
| Actuator | 1 in roller lever | Gives a forgiving, low-force trip point compared to a plain pin plunger, and tolerates a range of approach angles from the carriage |

## Wiring used in this build

```
Arduino pin (INPUT_PULLUP) ── switch COM
                               switch NC ── GND
```

See `04_Wiring_Electrical.md` for the full explanation of why NC (not NO)
is used, and how this makes a broken wire fail safely (read as "triggered,"
same as actually reaching the end of travel).

## How to test this part in isolation (bench test)

1. With the switch disconnected from the rig, use a multimeter in
   continuity mode across COM and NC.
2. At rest (lever not pressed): should read continuity (closed circuit,
   near 0 ohms).
3. With the lever pressed: should read open circuit (no continuity).
4. If this doesn't match, the switch itself has failed (rare for this
   switch type, but roller mechanisms can jam with debris) -- clean the
   roller pivot or replace the switch.

## Adjusting trip position

The switch's mounting bracket, not the switch itself, sets where along the
rail it trips (see `03_Assembly_Instructions.md` Step 6). If the trip point
needs to move, loosen the bracket's mounting screws and slide it along the
extrusion's T-slot rather than trying to adjust anything on the switch.

## Sourcing a replacement

Stocked by DigiKey, Amazon, and most industrial electrical suppliers under
the same Omron part number. Any SPDT roller-lever microswitch with an NC
contact and a current rating comfortably above what your logic circuit
draws (this one's 15A rating is overkill for that, deliberately) will work
as a drop-in replacement.
