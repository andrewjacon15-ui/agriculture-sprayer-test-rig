# Mean Well LRS-150-12 -- Enclosed Switching Power Supply

**Role in this rig:** converts AC mains power into the regulated 12V DC
that runs the motor (through the E-stop and driver).

**Manufacturer product page:** https://www.meanwell.com/ (search
"LRS-150-12") -- Mean Well publishes a full datasheet PDF directly from
their product page; that is the authoritative source for wiring terminal
labels and safety/compliance ratings.

## Key specs used in this design

| Spec | Value | Why it matters here |
|---|---|---|
| Output voltage | 12V DC | Matches the motor's rated voltage |
| Output current | 12.5A max | Far above the motor's 5A stall current -- generous headroom, and shared with nothing else in this design |
| Output power | 150W | |
| Input | Universal AC mains (~90-264VAC) | Works on standard wall power without a separate voltage selector |
| Enclosure | Fully enclosed metal case | Safer to have on an open benchtop rig than an open-frame supply |

## Wiring used in this build

- AC input terminals -> IEC power cord (BOM item 23).
- DC output `V+`/`V-` (exact terminal labels per the unit's markings) ->
  inline fuse holder -> E-stop -> Cytron MD13S `VM+`/`GND`. See
  `04_Wiring_Electrical.md` for the full chain.
- Most Mean Well enclosed supplies include a small onboard potentiometer
  (often labeled `V.ADJ`) that allows trimming the output voltage a couple
  of volts either side of 12V. This build doesn't need that adjustment, but
  it's useful to know it's there if you're troubleshooting a speed that's
  slightly off and want to rule out supply voltage as the cause -- measure
  the actual output with a multimeter before assuming it's adjustable.

## How to test this part in isolation (bench test)

1. With nothing connected to the DC output, plug in AC power.
2. Measure the DC output terminals with a multimeter -- should read close
   to 12V (typically within a percent or two).
3. If it reads 0V: check the AC input fuse (many of these supplies have an
   internal fuse, sometimes user-replaceable -- check the datasheet), and
   confirm the AC outlet itself is live.

## Safety note

This part carries mains voltage internally even though its output is a
safe low-voltage 12V. Never open the enclosure while it's plugged in, and
disconnect AC power before doing any wiring work downstream of it (see
`09_Safety.md`).

## Sourcing a replacement

Any enclosed 12V DC supply rated for at least ~5-6A continuous (comfortable
margin over the motor's stall current) from a reputable manufacturer
(Mean Well, similar) will work as a drop-in replacement. Bigger max-current
ratings are fine and common -- they just mean more headroom, not a
mismatch.
