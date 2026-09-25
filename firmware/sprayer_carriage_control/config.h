// =============================================================================
// config.h  --  All the "knobs" for the sprayer carriage rig live in this file.
//
// You should almost never need to touch the main .ino file. Instead, tune
// the rig by changing the numbers below and re-uploading. Every value has a
// plain-English explanation of what it does and why it's set the way it is.
// =============================================================================
#ifndef CONFIG_H
#define CONFIG_H

// -----------------------------------------------------------------------------
// 1. DOES YOUR MOTOR HAVE AN ENCODER WIRED UP?
// -----------------------------------------------------------------------------
// true  = the rig reads the motor's encoder and actively corrects speed in
//         real time (closed-loop control). More accurate, holds the ±10%
//         speed tolerance even if the battery sags or friction changes.
// false = the rig just applies a fixed, pre-calibrated PWM value for each
//         speed setting and trusts the motor to hold roughly that speed
//         (open-loop control). Simpler, works with a plain gearmotor that
//         has no encoder, but you must calibrate PWM_* values by hand with
//         a stopwatch (see docs/06_Calibration_and_Tuning.md).
#define ENCODER_ENABLED true

// -----------------------------------------------------------------------------
// 2. PIN MAP -- which Arduino Uno pin does each wire connect to
// -----------------------------------------------------------------------------
// Motor driver (Cytron MD13S, PWM + DIR interface)
#define PIN_MOTOR_PWM      9    // must be a PWM-capable pin (3,5,6,9,10,11 on Uno)
#define PIN_MOTOR_DIR      8

// Limit switches, wired using their NC (Normally Closed) contact -- see
// docs/04_Wiring_Electrical.md for exactly why. Reads LOW (switch's NC
// contact pulling the pin to GND) during normal travel, goes HIGH the
// instant the carriage reaches that end OR if a wire falls off -- both
// cases stop the motor, which is the safe failure mode.
#define PIN_LIMIT_HOME     6    // switch at the "start" end of the rail
#define PIN_LIMIT_FAR      7    // switch at the far end of the rail

// Quadrature encoder (only used if ENCODER_ENABLED is true)
#define PIN_ENCODER_A      2    // must be pin 2 or 3 on an Uno (hardware interrupt)
#define PIN_ENCODER_B      4

// Operator controls
#define PIN_BUTTON_MODE    11   // press to cycle LOW -> NOMINAL -> MAX -> LOW ...
#define PIN_BUTTON_START   12   // press to start/stop the automatic back-and-forth run
#define PIN_LED_LOW        A0
#define PIN_LED_NOMINAL    A1
#define PIN_LED_MAX        A2
#define PIN_LED_HEARTBEAT  13   // built-in LED, blinks whenever the main loop is alive

// -----------------------------------------------------------------------------
// 3. MECHANICAL / GEOMETRY CONSTANTS
// -----------------------------------------------------------------------------
// The rig is built so the LIMIT SWITCHES sit beyond the 6 ft measured zone,
// giving the carriage room to accelerate up to speed BEFORE it enters the
// zone you actually care about, and room to coast/stop AFTER it leaves the
// zone. See docs/01_System_Overview.md for the full reasoning.
#define MEASURED_TRAVEL_IN   72.0   // the 6 ft (72 in) zone speed is measured/held over
#define BUFFER_IN            12.0   // extra travel added on EACH end for accel/decel
// Total physical rail length = 72 + 12 + 12 = 96 in (8 ft)

// Drivetrain geometry: 80-tooth GT2 pulley (2 mm pitch) on the motor's output
// shaft. Circumference = 80 teeth * 2 mm = 160 mm = 6.2992 in.
#define PULLEY_CIRCUMFERENCE_IN  6.2992

// Gearmotor ratio (Pololu 19:1, 37Dx52L). Used to convert encoder counts on
// the motor's pre-gearbox shaft into output-shaft revolutions.
#define GEARBOX_RATIO       19.0
#define ENCODER_CPR         64.0   // counts per revolution of the MOTOR shaft (pre-gearbox)
// Counts per revolution of the OUTPUT shaft = ENCODER_CPR * GEARBOX_RATIO = 1216
// Counts per inch of belt travel = 1216 / PULLEY_CIRCUMFERENCE_IN ~= 193.06

// -----------------------------------------------------------------------------
// 4. SPEED SETTINGS
// -----------------------------------------------------------------------------
// Real-world target speeds, in miles per hour. Convert to inches/second in
// the firmware automatically (1 mph = 17.6 in/s).
#define SPEED_LOW_MPH        0.5
#define SPEED_NOMINAL_MPH    1.5   // the "desired" operating speed
#define SPEED_MAX_MPH        3.0   // the rig's rated top speed

// Open-loop PWM values (0-255) used when ENCODER_ENABLED is false, OR used
// as the STARTING GUESS for closed-loop control before the encoder trims it.
// These were sized from the motor's published no-load speed (500 RPM at
// 12 V) and the pulley above, which predicts ~35 duty% for LOW, ~50% for
// NOMINAL, and ~95% for MAX. Re-calibrate with a stopwatch after you build
// the rig -- see docs/06_Calibration_and_Tuning.md.
#define PWM_LOW              90
#define PWM_NOMINAL          128
#define PWM_MAX               242
#define PWM_MIN_MOVING        60   // below this, the motor may stall/cog instead of turning

// -----------------------------------------------------------------------------
// 5. MOTION PROFILE (acceleration / deceleration ramp)
// -----------------------------------------------------------------------------
// The carriage ramps PWM linearly from 0 up to the target duty cycle over
// RAMP_TIME_MS milliseconds at the start of every pass, and ramps back down
// over the same time before the far end. This is what "uses" the buffer
// distance configured above. See docs/01_System_Overview.md for the math
// that confirms 350 ms fits inside the 12 in buffer even at 3 mph.
#define RAMP_TIME_MS         350

// Small pause (ms) at each end of travel before reversing direction, so the
// motor fully stops and the mechanism settles before it starts moving the
// other way.
#define END_PAUSE_MS          400

// -----------------------------------------------------------------------------
// 6. CLOSED-LOOP (PI) CONTROL GAINS -- only used if ENCODER_ENABLED is true
// -----------------------------------------------------------------------------
// These adjust PWM during the CRUISE phase only (not during the accel/decel
// ramps) to correct for real-world friction, battery voltage, etc. so the
// measured speed matches the target within tolerance.
//   KP: how hard to react to a speed ERROR right now (proportional)
//   KI: how hard to react to an error that has PERSISTED over time (integral)
// Start conservative (small numbers). If the speed hunts/oscillates, lower
// them. If it settles too slowly, raise KP a little first.
#define SPEED_KP              6.0
#define SPEED_KI              2.0
#define SPEED_SAMPLE_MS       50   // how often (ms) the control loop re-checks speed

// -----------------------------------------------------------------------------
// 7. MISC
// -----------------------------------------------------------------------------
#define SERIAL_BAUD          115200
#define TELEMETRY_INTERVAL_MS 200  // how often status is printed to Serial Monitor

#endif // CONFIG_H
