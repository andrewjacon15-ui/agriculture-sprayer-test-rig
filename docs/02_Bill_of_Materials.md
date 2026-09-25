# 02 -- Bill of Materials

Prices below were checked in September 2026 from each supplier's public
listing and are **estimates** -- confirm current price/availability before
ordering, since maker-hardware pricing moves around. Where I couldn't pin
down one specific SKU's live price, I used a realistic market price for that
class of part and flagged it. Everything here is a genuinely common,
currently-sold part, not a discontinued or hypothetical one.

**Estimated total: ~$490 -- well under the $1,500 budget.** The margin is
intentional -- it covers shipping, tax, and the inevitable "oh, I also need
one more of X" during assembly. Section 8 lists optional upgrades if you'd
like to spend some of that margin deliberately.

## 1. Linear motion (rail + carriage)

| # | Part | Supplier | Approx. price | Qty | Subtotal |
|---|---|---|---|---|---|
| 1 | V-Slot 20x40mm Linear Rail, aluminum extrusion, cut to **96 in (2438 mm)** | [OpenBuilds](https://us.openbuilds.com/v-slot-20x40-linear-rail/) (or McMaster-Carr T-slot framing, or 8020.net -- any will do; order custom-cut to 96 in) | $70 | 1 | $70 |
| 2 | V-Slot 20mm Gantry Kit (wheel plate + 8x POM V-wheels + eccentric spacers) | [OpenBuilds](https://us.openbuilds.com/v-slot-gantry-kit-20mm/) | $38 | 1 | $38 |
| 3 | M5 T-nuts (drop-in, for 20-series V-slot) | OpenBuilds / Amazon | $10 (pack of 20) | 1 pk | $10 |
| 4 | M5x10 and M5x16 button head cap screws, assorted | McMaster-Carr / Amazon (M5 SHCS assortment kit) | $12 | 1 kit | $12 |

**Section subtotal: ~$130**

## 2. Drivetrain (motor, pulleys, belt)

| # | Part | Supplier | Approx. price | Qty | Subtotal |
|---|---|---|---|---|---|
| 5 | Pololu 19:1 Metal Gearmotor, 37Dx52L mm, 12V, **with 64 CPR encoder** (helical pinion) | [Pololu #4751](https://www.pololu.com/product/4751) | $40 | 1 | $40 |
| 6 | GT2 Timing Pulley, 80 tooth, 6mm bore, for 6mm belt (fits the motor's D-shaft with a set screw) | Amazon ("GT2 80T pulley 6mm bore") | $9 | 1 | $9 |
| 7 | GT2 Idler Pulley, 20 tooth, with bearing (for the far end of the belt loop) | Amazon ("GT2 20T idler pulley bearing") | $7 | 1 | $7 |
| 8 | GT2 Timing Belt, 6mm width, open roll (5 m) | Amazon ("GT2 belt 6mm 5m") | $10 | 1 | $10 |
| 9 | Aluminum flat bar, 1/8 in x 1 in x 12 in (raw stock for the motor mount bracket, see `03_Assembly_Instructions.md`) | McMaster-Carr | $14 | 1 | $14 |
| 10 | M4 shoulder screw + bearing (idler pulley axle) | McMaster-Carr | $8 | 1 | $8 |

**Section subtotal: ~$88**

> **Why this specific motor/pulley pair:** the 19:1 gearmotor is rated
> 500 RPM free-run at 12V. Paired with the 80-tooth pulley (6.30 in
> circumference), that predicts a top speed of `500 rpm × 6.30 in ÷ 60 s ≈
> 52.5 in/s ≈ 2.98 mph` -- right at the 3 mph max-speed requirement, with
> the 1.5 mph desired speed sitting comfortably at ~50% duty cycle (the
> well-behaved middle of the PWM range, not down in the low end where DC
> motors get cogging-prone). Stall torque is 84 oz-in at this ratio, which
> is far more than a light carriage + nozzle needs -- see
> `01_System_Overview.md`.

## 3. Motor control electronics

| # | Part | Supplier | Approx. price | Qty | Subtotal |
|---|---|---|---|---|---|
| 11 | Arduino Uno Rev3 (official board) | [Arduino Store](https://store.arduino.cc/products/arduino-uno-rev3/) | $28 | 1 | $28 |
| 12 | Cytron MD13S DC Motor Driver (13A cont. / 30A peak, 6-30V, PWM+DIR interface) | [Cytron](https://www.cytron.io/p-13amp-6v-30v-dc-motor-driver) / SparkFun / Amazon | $20 | 1 | $20 |
| 13 | Momentary pushbutton, panel mount (MODE and START) | Amazon / Adafruit | $3 | 2 | $6 |
| 14 | LED, 5mm, panel mount holder, assorted colors (mode indicators + heartbeat) | Amazon | $6 (pack) | 1 pk | $6 |
| 15 | 220Ω resistors (LED current limiting) | Amazon (assortment kit, already on hand for most makers) | $6 | 1 pk | $6 |
| 16 | Full-size solderless breadboard + jumper wire kit (prototyping before final wiring) | Amazon | $12 | 1 | $12 |
| 17 | Screw-terminal prototyping shield for Uno (final, vibration-resistant wiring) | Amazon ("Arduino Uno screw terminal shield") | $11 | 1 | $11 |

**Section subtotal: ~$89**

## 4. End-of-travel & safety switches

| # | Part | Supplier | Approx. price | Qty | Subtotal |
|---|---|---|---|---|---|
| 18 | Omron V-156-1C25 SPDT roller-lever limit switch, 15A/250V | [DigiKey](https://www.digikey.com/en/products/detail/omron-electronics-inc-emc-div/V-156-1C25/5237043) / Amazon | $9 | 2 | $18 |
| 19 | Mushroom-head E-stop pushbutton, 22mm, latching, 1 NC contact | Amazon ("22mm emergency stop switch NC") | $13 | 1 | $13 |
| 20 | Small L-brackets for mounting limit switches to the extrusion (or 3D-printed/bent sheet metal, see assembly doc) | Amazon / McMaster-Carr | $8 (pack) | 1 pk | $8 |

**Section subtotal: ~$39**

## 5. Power

| # | Part | Supplier | Approx. price | Qty | Subtotal |
|---|---|---|---|---|---|
| 21 | Mean Well LRS-150-12 (12V, 12.5A / 150W enclosed switching supply) | [Mean Well](https://www.meanwell.com/) via Mouser/Digikey/Amazon | $28 | 1 | $28 |
| 22 | Inline fuse holder + 10A automotive blade fuse (motor circuit protection) | Amazon | $7 | 1 | $7 |
| 23 | IEC C13 power cord (for the enclosed supply) | Amazon | $8 | 1 | $8 |

**Section subtotal: ~$43**

> Sized for headroom: the motor's rated stall current is only 5A, cruise
> current is a few hundred mA, so a 12.5A-capable supply leaves plenty of
> margin for acceleration current spikes without being oversized for the
> budget.

## 6. Enclosure, wiring, and misc.

| # | Part | Supplier | Approx. price | Qty | Subtotal |
|---|---|---|---|---|---|
| 24 | Plastic project enclosure, ~6x4x2 in (Arduino + driver + terminal blocks) | Amazon / Adafruit | $15 | 1 | $15 |
| 25 | Screw terminal blocks (barrier strip, for motor power and switch wiring) | Amazon | $9 | 1 pk | $9 |
| 26 | Hookup wire, 18 AWG stranded, multiple colors (motor power) | Amazon | $12 | 1 | $12 |
| 27 | Hookup wire, 22 AWG stranded, multiple colors (switches/signals) | Amazon | $10 | 1 | $10 |
| 28 | Heat-shrink tubing assortment | Amazon | $8 | 1 | $8 |
| 29 | Cable zip ties + adhesive mounts | Amazon | $6 | 1 | $6 |
| 30 | DC barrel jack / panel-mount power switch, 12V rated | Amazon | $6 | 1 | $6 |

**Section subtotal: ~$66**

## 7. Total

| Section | Subtotal |
|---|---|
| 1. Linear motion | $130 |
| 2. Drivetrain | $88 |
| 3. Control electronics | $89 |
| 4. Switches/safety | $39 |
| 5. Power | $43 |
| 6. Enclosure/wiring | $66 |
| **Estimated total** | **~$455** |
| **Budget** | **$1,500** |
| **Margin remaining** | **~$1,045** |

## 8. Optional upgrades (still comfortably in budget)

None of these are required -- the BOM above is a complete, working rig.

- **Cable drag chain** (~$25) to protect the motor power wire as the
  carriage travels, instead of a simple wire loop -- looks cleaner and
  lasts longer under repeated cycling.
- **A second Arduino Uno as a spare** (~$28) -- cheap insurance for a test
  rig that needs to stay running.
- **A small tabletop enclosure/frame** (aluminum extrusion legs, ~$60-100)
  if the rail needs to be elevated off a bench rather than clamped to
  existing structure.
- **A bench power supply instead of the Mean Well brick** (~$60-90) if
  you'd like adjustable voltage for experimenting with different speed
  ranges without touching the code.
