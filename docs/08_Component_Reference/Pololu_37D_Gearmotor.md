# Pololu 19:1 Metal Gearmotor, 37Dx52L mm, 12V, with 64 CPR Encoder

**Role in this rig:** the sole source of motion. Turns the drive pulley
that pulls the carriage along the belt.

**Manufacturer part page:** https://www.pololu.com/product/4751
**Manufacturer general datasheet (all 37D ratios):**
https://www.pololu.com/file/0J1736/pololu-37d-metal-gearmotors-rev-1-2.pdf

## Key specs used in this design

| Spec | Value | Where it's used |
|---|---|---|
| Rated voltage | 12V | Matches the Mean Well supply |
| Free-run speed @ 12V | 500 RPM | Top-speed calculation, `01_System_Overview.md` |
| Free-run current @ 12V | ~300 mA | Sanity-checking supply sizing |
| Stall torque @ 12V | 84 oz-in (5.9 lb-in) | Confirms huge margin over the light carriage load |
| Stall current @ 12V | ~5A | Fuse and driver current-rating selection |
| Gear ratio | 19:1 | Used in `config.h` (`GEARBOX_RATIO`) for the encoder-to-distance math |
| Encoder resolution | 64 counts/rev of the **motor shaft** (pre-gearbox) | Combined with the gear ratio: 64 x 19 = **1216 counts per revolution of the output/pulley shaft** |
| Output shaft | 6mm diameter D-shaft (flat for set-screw purchase) | Matches the GT2 pulley's 6mm bore + set screw |

## Electrical connections

- **Motor power leads**: two wires (no polarity marking required for a DC
  brushed motor -- swapping them just reverses rotation direction, which is
  how you fix a "spins the wrong way" issue per
  `04_Wiring_Electrical.md`). Connect to Cytron MD13S `M1A`/`M1B`.
- **Encoder cable**: a JST-ZH style connector with leads for GND, Vcc
  (3.5-20V logic supply), and two quadrature outputs (channel A, channel
  B). Connect Vcc to Arduino 5V, GND to Arduino GND, and A/B to Arduino
  D2/D4 (see `04_Wiring_Electrical.md`). **Confirm the exact pinout/wire
  colors against the leaflet packed with your specific unit** -- Pololu has
  used slightly different connector pinouts across product revisions of
  this motor family, and the printed leaflet in the box is the definitive
  source for the exact unit you received.

## Mechanical mounting

The 37D gearbox housing is round with a flat mounting face containing
threaded holes for attaching a bracket. **Do not fabricate the motor
mount bracket (see `03_Assembly_Instructions.md` Step 3) from a remembered
or assumed hole spacing** -- get the exact bolt pattern and shaft diameter
from either:

1. The mechanical drawing on the Pololu product page (link above, look for
   the dimensioned diagram), or
2. Direct measurement of your physical unit with calipers before drilling.

## How to test this part in isolation (bench test, off the rig)

Useful if you suspect the motor itself rather than the wiring/driver:

1. Disconnect the motor leads from the Cytron driver.
2. Touch the two motor leads briefly and directly across a 9V or 12V
   battery (a quick touch, not a sustained connection, is enough to
   confirm the motor spins). It should spin freely and reverse when you
   swap which lead touches which terminal.
3. To check the encoder independently, connect only its GND/Vcc/A/B leads
   to the Arduino as described above (motor leads can stay disconnected),
   upload a minimal test sketch that just prints the `A`/`B` pin states or
   a running count while you spin the output shaft by hand, and confirm the
   count changes and reverses direction correctly.

## Sourcing a replacement

This is a stocked Pololu catalog item, not a custom part -- reorder the
identical product page above. If it's discontinued by the time you need a
replacement, search Pololu's "37D Metal Gearmotors" category for the
closest available 19:1 (or nearby ratio) 12V option with an encoder, and
re-verify the free-run RPM and stall torque against the table above before
assuming it's a drop-in match -- if the RPM differs, revisit the pulley
sizing math in `01_System_Overview.md`.
