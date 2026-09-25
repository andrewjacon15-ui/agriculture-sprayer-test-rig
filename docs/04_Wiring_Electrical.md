# 04 -- Wiring & Electrical

## Power domains (read this first)

This rig deliberately uses **two separate power paths**:

1. **12V motor power**: Mean Well supply -> E-stop -> Cytron MD13S -> motor.
   The E-stop physically interrupts this path. Nothing downstream of the
   E-stop can move the motor while it's pressed, regardless of what the
   firmware does.
2. **Arduino logic power**: USB from a computer, or a separate 5V wall
   adapter into the Arduino's own power jack. This is intentionally
   independent of the 12V line, so the Arduino keeps running (and keeps
   printing status to the Serial Monitor) even when the E-stop has cut
   motor power. That's how you can tell "E-stop is pressed" apart from
   "everything just died."

The Cytron MD13S's own 5V logic rail (broken out on its header) is **not**
used to power the Arduino in this design, to keep the two domains cleanly
separate. Just share a common ground between the Arduino and the driver
board (see diagram) so the PWM/DIR signals read correctly.

## System wiring diagram

```
                      ┌────────────────────┐
  AC mains ─────────▶ │ Mean Well LRS-150-12│
                      └─────────┬───────────┘
                                │ 12V / GND
                                ▼
                      ┌───────────────────┐
                      │   E-STOP (NC)      │   <- physically interrupts
                      └─────────┬──────────┘      12V+ only
                                │
                                ▼
                      ┌───────────────────┐
                      │  Inline fuse (10A) │
                      └─────────┬──────────┘
                                │
                                ▼
                   ┌────────────────────────┐
                   │   Cytron MD13S          │
                   │   VM+ / GND             │
                   │                         │
                   │   M1A/M1B ───────────┐  │
                   │   PWM ◀── D9  Uno     │  │
                   │   DIR ◀── D8  Uno     │  │
                   │   GND ── GND  Uno     │  │
                   └──────────────────┬────┘  │
                                       │       │
                                       ▼       ▼
                                 ┌───────────────────┐
                                 │  Pololu 37D motor  │
                                 │  (+ encoder)        │
                                 └──────────┬──────────┘
                                            │ A / B / 5V / GND
                                            ▼
                                    Arduino D2 / D4 / 5V / GND

  Limit switch HOME  (NC, COM) ──── D6  Uno  /  GND
  Limit switch FAR   (NC, COM) ──── D7  Uno  /  GND
  MODE button                   ──── D11 Uno  /  GND
  START button                  ──── D12 Uno  /  GND
  LED LOW    (+ 220Ω) ──────────── A0  Uno
  LED NOMINAL(+ 220Ω) ──────────── A1  Uno
  LED MAX    (+ 220Ω) ──────────── A2  Uno

  Arduino 5V/logic power: USB (from PC) or separate 5V wall adapter
  -- NOT from the 12V motor rail.
```

## Arduino Uno pin table

| Uno pin | Connects to | Direction | Notes |
|---|---|---|---|
| D2 | Encoder channel A | input | must be D2 or D3 (hardware interrupt) |
| D4 | Encoder channel B | input | |
| D6 | HOME limit switch (COM) | input, `INPUT_PULLUP` | switch's other leg -> GND |
| D7 | FAR limit switch (COM) | input, `INPUT_PULLUP` | switch's other leg -> GND |
| D8 | Cytron `DIR` | output | |
| D9 | Cytron `PWM` | output | must be a PWM pin |
| D11 | MODE button | input, `INPUT_PULLUP` | other leg -> GND |
| D12 | START/STOP button | input, `INPUT_PULLUP` | other leg -> GND |
| D13 | Heartbeat LED | output | Uno's built-in LED, no extra part needed |
| A0 | LOW-speed LED | output | through 220Ω resistor to GND |
| A1 | NOMINAL-speed LED | output | through 220Ω resistor to GND |
| A2 | MAX-speed LED | output | through 220Ω resistor to GND |
| 5V | Encoder Vcc | output | |
| GND | Common ground | -- | tie Arduino GND, Cytron GND, and switch/button grounds together |

If you build with these pins reassigned, update the matching `PIN_*`
`#define`s in `firmware/sprayer_carriage_control/config.h` -- the firmware
and this table need to agree.

## Limit switches: why NC (Normally Closed), and how to wire them

Each Omron V-156 switch has three terminals: **COM**, **NO** (normally
open), and **NC** (normally closed). **Wire using COM and NC** — leave NO
disconnected.

```
Uno pin (INPUT_PULLUP) ── switch COM
                            switch NC ── GND
```

Walk through what this does in each situation:

| Situation | COM-NC circuit | Uno pin reads | Firmware sees |
|---|---|---|---|
| Normal travel (lever not pressed) | Closed (this is what "Normally Closed" means) | pulled to GND -> **LOW** | not triggered -- OK to run |
| Carriage reaches that end (lever pressed) | Opens | pullup takes over -> **HIGH** | triggered -- stop |
| A wire falls off, breaks, or is unplugged | Opens (same as above) | pullup takes over -> **HIGH** | triggered -- stop |

That last row is the whole point of choosing NC over NO: a wiring fault
looks electrically identical to "the carriage reached the end," so the rig
fails toward stopping, never toward silently ignoring a real limit. This
matches `limitTriggered()` in the firmware, which treats **HIGH as
triggered** and **LOW as normal** — no code changes needed if you wire it
as shown above.

**Verify it before you trust it.** Watch the `homeSw=`/`farSw=` fields in
the Serial Monitor telemetry: press each switch by hand and confirm the
field changes to `TRIG`, then unplug that switch's signal wire and confirm
it *also* shows `TRIG` (not `open`). If unplugging a wire shows `open`
instead, that switch is wired to its NO contact instead of NC — move the
wire to the NC terminal.

## Motor direction

`DIR` pin HIGH drives the carriage toward the FAR end; LOW drives it toward
HOME (see `config.h` comments). If your first test run drives the wrong way
during homing, **don't edit the code** -- just swap the two motor leads on
the Cytron `M1A`/`M1B` terminals, which reverses the motor's physical
rotation direction to match the firmware's assumption.

## Encoder direction

Similarly, if the measured speed telemetry goes negative or the closed-loop
control seems to fight itself (motor speeds up then suddenly cuts, or never
settles), the encoder's A/B channels are probably swapped relative to the
motor's rotation direction. Swap the two encoder signal wires (A and B) at
the Arduino end -- do not edit the firmware.
