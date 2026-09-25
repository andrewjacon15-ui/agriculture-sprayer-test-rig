# 00 -- Start Here (Beginner Setup Guide)

**This page assumes you've never done anything like this before.** No
electronics background, no programming background, nothing installed on
your computer. Read this whole page once before you buy or build anything
-- it tells you every tool and every piece of software you'll need, and the
order to do things in, so you're not stopping mid-project to go find
something.

If you *do* have experience with this stuff, skip straight to
`01_System_Overview.md` -- this page is the hand-holding version.

## What you're building, in 30 seconds

A motorized cart that rides back and forth on an 8 ft rail at a controlled
speed, to test a water sprayer. You'll assemble some aluminum rail and a
motor (mechanical work, like assembling furniture), wire up some
electronics (connecting labeled wires to labeled terminals, no soldering
required to have a working rig), and load a program onto a small
controller board called an Arduino (copy-paste some settings, click
"Upload"). None of these three things require prior experience -- each one
is just slower the first time.

## The complete tool list

Everything below is cheap and available at any hardware store, electronics
retailer (Amazon, Micro Center, etc.), or is free to download. Buy/download
everything on this list before you start Step 1 in
`03_Assembly_Instructions.md`, so you're not mid-build waiting on a
delivery.

### Mechanical tools

| Tool | What it is | Do you need it? |
|---|---|---|
| Hex keys (Allen keys), 2.5mm and 4mm | Small L-shaped metal tools with a 6-sided tip, used to tighten the bolts that hold aluminum rail systems together. Usually **included free** with the V-slot hardware kits in the BOM. | Check your parts first before buying a set |
| Phillips screwdriver | The standard cross-head screwdriver everyone has a version of | Yes |
| Drill + drill bits | For drilling the two mounting holes in the motor bracket (Assembly Step 3) | Yes |
| Hacksaw, or a way to cut aluminum bar stock | To cut the motor bracket material to length | Yes -- **or** ask the hardware/metal store to cut it to your measurement when you buy it, which most will do for free or a couple dollars. This is the easiest option if you don't own a saw. |
| Tape measure or ruler | For marking the 12 in / 84 in reference marks on the rail | Yes |
| A vice, or just a second pair of hands | Makes drilling/cutting the bracket much easier | Helpful, not required |

### Electrical tools

| Tool | What it is | Do you need it? |
|---|---|---|
| Wire strippers | A small pliers-like tool that strips the plastic insulation off the end of a wire without cutting the metal inside. A cheap $8-10 one is fine. | Yes |
| Small screwdriver (flathead, ~3mm tip) | For tightening the screw-terminal blocks (where wires clamp in with a screw, no soldering) | Yes |
| Multimeter | A handheld meter that reads voltage and checks whether a wire/switch is making a connection ("continuity"). This is the single most useful tool for confirming your wiring is right *before* you power anything on, and for troubleshooting later. A basic one is $15-20. **Get one -- the wiring and troubleshooting docs assume you have it.** | Yes |
| Soldering iron + solder | Melts a metal alloy to permanently join two wires | **Optional.** This build is designed to be done entirely with screw terminals -- see `04_Wiring_Electrical.md`. Solder a connection only if you want a more permanent joint somewhere; nothing in this project requires it. |
| Wire cutters | For trimming wire to length | Yes (wire strippers usually include a cutting edge, so this may be the same tool) |

**How to use a multimeter, in one paragraph:** Turn the dial to the
symbol that looks like a sound wave or has a little speaker icon next to
it -- that's "continuity mode." Touch the two probe tips together; you
should hear a beep, confirming the meter works. Now touch one probe to
each end of a wire or switch: a beep means current can flow through it
(closed circuit); silence means it can't (open circuit). To measure
voltage instead, turn the dial to the "V" with a straight line near it
(DC volts), pick a range above what you're measuring (20V range for a 12V
circuit), and touch the red probe to positive and black probe to negative
-- the screen shows the voltage.

### Software (all free)

| Software | What it's for | Where to get it |
|---|---|---|
| **Arduino IDE** | The program you use to write and upload the control code to the Arduino board | arduino.cc -- full install walkthrough below |

That's genuinely the only software this project needs. No account
signup, no subscription, no CAD software required to build this exactly
as documented.

## Installing the Arduino IDE

Do this now, before you've even ordered parts -- it takes 10 minutes and
lets you make sure your computer can talk to an Arduino before you're
relying on it to work mid-build.

### Windows

1. Open a web browser and go to **arduino.cc**.
2. Click **Software** in the top menu (or go to arduino.cc/en/software).
3. Under "Windows," click the installer download link (it will offer you
   a choice between the Microsoft Store version and a plain `.exe`
   installer -- either works; the plain installer is simpler if you're
   unsure).
4. Once the download finishes, look in your **Downloads** folder for a
   file that starts with `arduino-ide` and ends in `.exe`. Double-click
   it.
5. Windows may show a blue "Windows protected your PC" security warning
   because the installer isn't from the Microsoft Store. Click **More
   info**, then **Run anyway**. This is normal for legitimate software
   downloaded directly from a developer's website.
6. Click through the installer (Next, Next, I Agree, Install). When it
   asks about installing drivers, click **Install** / **Yes** on any
   driver prompts that appear -- these let Windows recognize the Arduino
   when you plug it in.
7. When it finishes, an Arduino icon will be on your desktop or in your
   Start menu. Open it once to confirm it launches -- you'll see a mostly
   blank code editor window. That's it, installed correctly.

### Mac

1. Open a web browser and go to **arduino.cc**.
2. Click **Software** in the top menu (or go to arduino.cc/en/software).
3. Under "macOS," click the download link.
4. Once downloaded, open your **Downloads** folder and double-click the
   file (it will look like a disk image / `.dmg` file).
5. A window will pop up showing the Arduino IDE icon and a shortcut to
   your Applications folder. Drag the Arduino icon onto the Applications
   folder icon to install it.
6. Open your **Applications** folder and double-click **Arduino IDE**.
7. macOS will likely warn "Arduino IDE is an app downloaded from the
   Internet, are you sure you want to open it?" Click **Open**. (If macOS
   blocks it entirely, go to **System Settings > Privacy & Security**,
   scroll down, and click **Open Anyway** next to the Arduino mention.)
8. The editor window should open. That's it, installed correctly.

### Confirming it works (do this once you have the Arduino board in hand)

1. Plug the Arduino Uno into your computer with a USB cable.
2. Open the Arduino IDE.
3. Go to **Tools > Board** and select **Arduino Uno**.
4. Go to **Tools > Port**. A new option should have appeared in this menu
   the moment you plugged in the board (on Windows, something like
   `COM3`; on Mac, something like `/dev/cu.usbmodem...`). Select it.
   - **If no new port shows up:** unplug the USB cable, wait 5 seconds,
     plug it back in, and check the menu again. If it still doesn't show
     up, try a different USB cable -- some cheap USB cables are
     "charge-only" and can't transfer data, which is a surprisingly
     common gotcha.
5. Go to **File > Examples > 01.Basics > Blink**, then click the
   right-arrow **Upload** button. After a few seconds you should see
   "Done uploading" and the small LED on the Arduino board itself should
   start blinking once per second.

If that LED is blinking, your computer, your Arduino, and your Arduino
IDE installation are all confirmed working -- you're ready to load the
actual project firmware later in `firmware/sprayer_carriage_control/README.md`.

## Recommended build order

Work through the project in this order. Each step links to the doc that
covers it.

- [ ] **Read `01_System_Overview.md`** -- 10 minutes, no tools needed.
  Explains *why* the rig is built the way it is, which makes every later
  step make more sense.
- [ ] **Order everything in `02_Bill_of_Materials.md`.** Expect 3-10 days
  for parts to arrive depending on suppliers.
- [ ] **Install the Arduino IDE** (above), if you haven't already.
- [ ] **Mechanical assembly -- `03_Assembly_Instructions.md`, Steps 1-6.**
  Building the rail, carriage, motor bracket, belt, and limit switches.
  Budget a few hours; this is the longest single stretch of work.
- [ ] **Electrical wiring -- `03_Assembly_Instructions.md` Step 7, using
  `04_Wiring_Electrical.md` as the reference.** Connecting everything with
  screw terminals. Go slow here and double-check with your multimeter
  before powering anything on.
- [ ] **Upload the firmware -- `firmware/sprayer_carriage_control/README.md`.**
  Open the sketch in the Arduino IDE and click Upload.
- [ ] **First power-on checklist -- `03_Assembly_Instructions.md` Step 8.**
  Done with the belt *disconnected* from the carriage, on purpose, so a
  surprise doesn't fling anything.
- [ ] **Final assembly -- `03_Assembly_Instructions.md` Step 9.**
  Reconnect the belt, mount your sprayer hardware.
- [ ] **Calibrate -- `06_Calibration_and_Tuning.md`.** Confirm the rig
  actually hits its target speeds within ±10% using a stopwatch.
- [ ] **Read `09_Safety.md`** before running it around other people.

If something doesn't work at any step, `07_Troubleshooting.md` is
organized by symptom (motor doesn't move, speed is inconsistent, etc.) --
check there before assuming you've done something wrong.

## Glossary

Plain-English definitions for the technical terms used throughout these
docs, so you don't have to look them up elsewhere.

| Term | Meaning |
|---|---|
| **Arduino / Arduino Uno** | A small, cheap circuit board with a built-in computer chip that you can program to read sensors and control motors. This project's "brain." |
| **Firmware / sketch** | The program that runs on the Arduino. "Sketch" is just Arduino's word for a program file. |
| **PWM (Pulse Width Modulation)** | A way of controlling motor speed by switching power on and off very rapidly. The more time it's "on" out of each cycle, the faster the motor spins. |
| **Duty cycle** | The percentage of time PWM power is "on." Written in this project as a number 0-255 (0 = always off, 255 = always on). |
| **H-bridge / motor driver** | A small circuit board (the Cytron MD13S in this build) that sits between the low-power Arduino and the high-power motor, so the Arduino's small signal can control the motor's much larger current safely. |
| **Encoder** | A sensor attached to the motor that reports how far and how fast the shaft has turned, by counting tiny pulses as it spins. Optional in this build; lets the firmware actively correct speed in real time. |
| **Closed-loop vs. open-loop control** | Closed-loop = the system measures its actual speed (via the encoder) and corrects itself. Open-loop = the system just applies a fixed setting and trusts it to work, with no feedback. |
| **Gear ratio** | How much the motor's gearbox trades speed for torque (turning force). A 19:1 ratio means the output shaft turns once for every 19 turns of the raw motor -- much slower, but far stronger. |
| **Stall torque** | The maximum twisting force a motor can produce right before it stops turning under too much load. Used to confirm the motor is strong enough for the job. |
| **RPM** | Revolutions per minute -- how fast something spins. |
| **GT2 belt / pulley** | A rubber belt with small square teeth (GT2 is the tooth shape/spacing standard) that meshes with a matching toothed pulley, so the belt can't slip the way a smooth belt would. |
| **V-slot extrusion** | Aluminum bar stock with a V-shaped groove running down each side, designed so wheeled carriages can roll smoothly along it. The "rail" in this project. |
| **T-nut** | A small nut shaped to slide into the slot of aluminum extrusion and grip when a bolt is tightened into it -- how everything bolts onto the rail. |
| **Limit switch** | A physical switch that gets pressed when something (the carriage) reaches it, used here to detect "the carriage has reached the end of the rail." |
| **NC / NO (Normally Closed / Normally Open)** | Describes a switch's resting state. NC = the circuit is connected until you press the switch. NO = the circuit is disconnected until you press it. This project uses NC switches because it's the safer failure mode -- see `04_Wiring_Electrical.md`. |
| **Continuity** | Whether a circuit is complete (current can flow through it). Your multimeter's "continuity mode" beeps when it detects this. |
| **E-stop (emergency stop)** | A large, easy-to-hit button that immediately cuts power to the motor, independent of any code. |
| **Debounce** | A small delay used in code to ignore the rapid, spurious on/off flickering that happens for a few milliseconds when you press a physical button or switch, so one press doesn't get read as ten. |
| **Baud rate** | The speed (in bits per second) at which the Arduino and your computer agree to talk over USB. Both sides must be set to the same number (115200 in this project) or the text you see will look like garbage characters. |
| **Telemetry** | The status information (speed, position, state) the Arduino continuously prints to your computer screen so you can see what it's doing. |
