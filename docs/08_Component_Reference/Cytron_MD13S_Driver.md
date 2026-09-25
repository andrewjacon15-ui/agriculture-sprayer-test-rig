# Cytron MD13S -- 13A DC Motor Driver

**Role in this rig:** sits between the Arduino (which only outputs small
signal-level voltages) and the motor (which needs real current at up to
12V). Takes a PWM speed command and a direction command from the Arduino
and switches the full motor current accordingly. This kind of board is
usually called an "H-bridge" driver.

**Manufacturer product page:** https://www.cytron.io/p-13amp-6v-30v-dc-motor-driver
**Manufacturer user manual/datasheet:** linked from the product page above
("User's Manual" / documentation tab) -- **treat that PDF as the
authoritative pinout reference**; the summary below is what this build
actually uses, not the full feature set of the board.

## Key specs used in this design

| Spec | Value | Why it matters here |
|---|---|---|
| Continuous current | 13A | Well above the motor's 5A stall current -- generous safety margin |
| Peak current | 30A | Covers momentary inrush at power-on |
| Input voltage range | 6-30V | Comfortably covers our 12V supply |
| Control interface | PWM + DIR (2 logic pins) | This is the simplest of Cytron's supported control modes and the one this firmware uses |
| Logic level | 3.3V-5V compatible | Matches the Arduino Uno's 5V logic directly, no level shifting needed |

## Terminals used in this build

| Terminal (board silkscreen) | Connects to | Notes |
|---|---|---|
| `VM+` | Fused 12V from the Mean Well supply (through the E-stop) | Motor power input |
| `GND` (power side) | Supply/E-stop return, and shared with Arduino GND | Common ground reference is required for PWM/DIR signals to read correctly |
| `M1A` / `M1B` | Motor's two power leads | Polarity doesn't matter for function; swapping them reverses rotation direction |
| `PWM` | Arduino `D9` | Speed command, 0-255 duty cycle from `analogWrite()` |
| `DIR` | Arduino `D8` | Direction command: HIGH/LOW picks rotation direction (see `04_Wiring_Electrical.md`) |
| `GND` (logic side) | Arduino `GND` | If the board has separate power/logic ground terminals, tie both to the same common ground |

**Confirm exact terminal labeling against your physical board's silkscreen
and the manufacturer's manual before wiring** -- terminal order can vary
slightly across board revisions even when the part number is the same.

## How to test this part in isolation (bench test)

1. Power the board's `VM+`/`GND` from the 12V supply (no motor connected
   yet), and connect `PWM`/`DIR`/`GND` to the Arduino as in normal
   operation.
2. Connect a low-current test load (or just the motor itself, disconnected
   from the belt so it spins freely) to `M1A`/`M1B`.
3. Upload a minimal test sketch that sets `DIR` HIGH and ramps `PWM` from 0
   to 255 with `analogWrite()`, then repeats with `DIR` LOW. Confirm the
   motor spins in both directions and speeds up smoothly with increasing
   PWM.
4. If the motor doesn't respond: check for 12V at `VM+`/`GND` first (rules
   out the power path), then check for a changing voltage at `PWM` with a
   multimeter (rules out the Arduino signal), before suspecting the driver
   board itself.

## Sourcing a replacement

Stocked by Cytron directly and multiple resellers (SparkFun, Amazon,
RobotShop). Any board that accepts a PWM + DIR interface, supports 12V
input, and is rated well above the motor's 5A stall current (the MD13S's
13A continuous rating) will work as a drop-in replacement -- you do not
need this exact model number, just those three characteristics.
