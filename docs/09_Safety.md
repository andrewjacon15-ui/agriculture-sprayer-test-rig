# 09 -- Safety

This is a test rig with a moving carriage on an exposed rail, driven by a
motor with enough torque to pinch a finger. Take it seriously even though
it's "just" a test fixture.

## Hardware E-stop is the primary safety control

The E-stop button physically interrupts the 12V motor power line, upstream
of the motor driver, before any wire reaches the Arduino (see
`04_Wiring_Electrical.md`). This is deliberate: **software should never be
the only thing standing between a moving motor and a stopped one.** If the
Arduino locks up, crashes, or is mid-upload, the E-stop still works,
because it doesn't run through the Arduino at all.

- Mount the E-stop somewhere reachable from wherever an operator normally
  stands while the rig is running -- not on the far side of the rail from
  where you'll actually be.
- Test it at the start of every session: press it, confirm the motor
  stops/can't be commanded to move, release it, and only then proceed.

## Limit switches are wired fail-safe

Both limit switches use their NC contact, so a broken wire reads exactly
like "the carriage reached the end" and stops the motor (see
`04_Wiring_Electrical.md` for the full explanation). This means a wiring
fault fails toward stopping the rig, not toward letting it run unchecked
into a hard stop.

## Pinch points

The belt-and-pulley drive has real pinch points at both pulleys and
anywhere the belt runs close to the carriage plate. Keep fingers, loose
clothing, and cabling clear of these while the rig is powered, and route
wiring (motor power, sprayer water line) so it can't get caught between the
carriage and the rail as it travels.

## Before every run

1. Confirm the rail and carriage are clear of tools, hands, and loose
   objects along the full 8 ft.
2. Confirm the E-stop works (see above).
3. Confirm both limit switches are seated and functioning (watch the
   `homeSw`/`farSw` telemetry while triggering each by hand -- see
   `05_Software_Operation_Guide.md`).
4. Start at LOW speed for the first pass of any new session or after any
   hardware change, and only move to NOMINAL/MAX once you've confirmed the
   rig is behaving as expected.

## Electrical

- The Mean Well supply is mains-powered. Keep it dry, and don't operate the
  rig around standing water despite this being a water sprayer test --
  route the sprayer's water line and any spray/overspray away from the
  electronics enclosure, power supply, and exposed wiring.
- Use the inline fuse specified in the BOM. It's there to protect the wiring
  and motor from a stall/jam condition drawing well above normal current --
  don't bypass or upsize it.
- Disconnect mains power before opening the electronics enclosure for any
  wiring changes.

## Water and electronics

This rig moves a water sprayer, so the whole build should assume occasional
overspray or drips near the rail. Recommended practice, not covered in the
BOM since it's site-specific:

- Mount the electronics enclosure above or well clear of the spray zone.
- Use a drip loop on any cabling that runs down toward the rail, so water
  running along the cable drips off before reaching a connector.
- If this rig will see regular water exposure, consider upgrading the
  project enclosure to an IP-rated one and using weatherproof cable glands
  -- both are inexpensive swaps within the existing budget margin (see BOM
  Section 8).
