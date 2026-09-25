/*
 * =============================================================================
 * Sprayer Carriage Speed-Test Rig -- Motor Control Firmware
 * =============================================================================
 *
 * WHAT THIS RIG DOES, IN PLAIN LANGUAGE
 * --------------------------------------
 * A carriage (carrying a water sprayer head) rides back and forth on an 8 ft
 * rail. A DC gearmotor pulls it along a toothed belt. Only the MIDDLE 6 ft of
 * that rail is where we actually care about speed -- the 12 in on each end
 * is "runway" the carriage uses to speed up before entering the 6 ft zone,
 * and to slow down after leaving it. A switch at each end of the rail tells
 * the rig "you've reached the end, stop and turn around."
 *
 * So each pass down the rail has three parts, like a car merging onto a
 * highway and then exiting it:
 *   1. ACCELERATE  (in the first 12 in buffer)
 *   2. CRUISE at the target speed (across the 6 ft measured zone)
 *   3. DECELERATE  (in the last 12 in buffer, until the limit switch confirms
 *      we've reached the end)
 * Then it pauses briefly, reverses, and does the same thing going back.
 *
 * HOW SPEED IS CONTROLLED
 * ------------------------
 * The motor is driven by PWM (Pulse Width Modulation) -- rapidly switching
 * the power on and off many times a second. The higher the percentage of
 * time it's "on" (the duty cycle), the faster the motor spins. Arduino
 * represents duty cycle as a number from 0 (off) to 255 (full on).
 *
 * If you wired up the optional encoder (ENCODER_ENABLED in config.h), the
 * firmware also counts how far the motor has actually turned and nudges the
 * PWM value up or down in real time to hold the exact target speed -- this
 * is "closed-loop control," and it's what most self-driving factory
 * equipment does. Without the encoder, the firmware just applies a fixed,
 * hand-calibrated PWM value and trusts the motor -- "open-loop control." See
 * docs/06_Calibration_and_Tuning.md for how to tune either one.
 *
 * SAFETY DESIGN
 * --------------
 * - The physical E-STOP button cuts 12V motor power in HARDWARE, upstream of
 *   everything below. Nothing in this code can override that -- that's on
 *   purpose. See docs/09_Safety.md.
 * - Limit switches are wired Normally-Closed. A broken wire looks exactly
 *   like "I've hit the end, stop" -- the safe direction to fail.
 * - The state machine below NEVER assumes it knows where the carriage is.
 *   It always waits for the physical switch before believing it has reached
 *   an end.
 *
 * This file only orchestrates WHEN things happen (the state machine). All
 * the tunable numbers (pins, speeds, distances, PID gains) live in
 * config.h -- start there if you want to change how the rig behaves.
 * =============================================================================
 */

#include "config.h"

// -----------------------------------------------------------------------------
// Derived constants (calculated once from config.h, not meant to be edited)
// -----------------------------------------------------------------------------
const float IN_PER_SEC_PER_MPH = 17.6;   // 1 mph = 17.6 inches/second
const float COUNTS_PER_INCH = (ENCODER_CPR * GEARBOX_RATIO) / PULLEY_CIRCUMFERENCE_IN;

const float SPEED_LOW_IN_S      = SPEED_LOW_MPH     * IN_PER_SEC_PER_MPH;
const float SPEED_NOMINAL_IN_S  = SPEED_NOMINAL_MPH * IN_PER_SEC_PER_MPH;
const float SPEED_MAX_IN_S      = SPEED_MAX_MPH     * IN_PER_SEC_PER_MPH;

// -----------------------------------------------------------------------------
// State machine definitions
// -----------------------------------------------------------------------------
enum RigState {
  STATE_HOMING,       // looking for the HOME limit switch at power-up
  STATE_IDLE,         // stopped, waiting for the operator to press START
  STATE_ACCEL,        // ramping speed up through the first buffer zone
  STATE_CRUISE,       // holding target speed through the measured 6 ft zone
  STATE_DECEL,        // ramping speed down through the last buffer zone
  STATE_END_PAUSE,    // brief stop at the end before reversing (or stopping)
  STATE_FAULT         // something didn't happen when expected -- see Serial
};

enum TravelDir {
  DIR_TOWARD_FAR,      // DIR pin HIGH -- away from the home switch
  DIR_TOWARD_HOME       // DIR pin LOW  -- back toward the home switch
};

enum SpeedMode { MODE_LOW, MODE_NOMINAL, MODE_MAX };

RigState  state = STATE_HOMING;
TravelDir direction = DIR_TOWARD_FAR;
SpeedMode speedMode = MODE_NOMINAL;
bool running = false;          // true once the operator presses START
bool faultLatched = false;

unsigned long phaseStartMs = 0;
unsigned long cruiseDurationMs = 0;
int rampStartPWM = 0;
int rampTargetPWM = 0;
float currentTargetSpeedInS = SPEED_NOMINAL_IN_S;
int commandedPWM = 0;

// Encoder bookkeeping (only meaningful if ENCODER_ENABLED)
volatile long encoderCount = 0;
long lastEncoderSampleCount = 0;
unsigned long lastSpeedSampleMs = 0;
float measuredSpeedInS = 0.0;
float speedIntegral = 0.0;

unsigned long lastTelemetryMs = 0;
unsigned long lastHeartbeatMs = 0;
bool heartbeatOn = false;

// Simple button debounce state
struct Button {
  uint8_t pin;
  bool lastReading;
  bool stableState;
  unsigned long lastChangeMs;
};
Button modeButton  = {PIN_BUTTON_MODE,  HIGH, HIGH, 0};
Button startButton = {PIN_BUTTON_START, HIGH, HIGH, 0};

// Each button is debounced exactly once per loop() iteration and the result
// cached here, because debouncedFallingEdge() has a side effect (it updates
// the button's "last known stable state"). Calling it twice in the same
// loop for the same button would let the first caller silently consume an
// edge the second caller needed to see -- e.g. clearing a FAULT with START
// would never register if both readButtons() and runFault() independently
// re-checked the raw button.
bool startEdgeThisLoop = false;
bool modeEdgeThisLoop = false;

// =============================================================================
// SETUP
// =============================================================================
void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println(F("=== Sprayer Carriage Rig booting ==="));

  pinMode(PIN_MOTOR_PWM, OUTPUT);
  pinMode(PIN_MOTOR_DIR, OUTPUT);

  // Limit switches are wired using their NC (Normally Closed) contact, with
  // the Uno's internal pullup enabled. At rest the NC contact is closed and
  // pulls the pin to GND ("not triggered" reads LOW); tripping the switch
  // OR a wire falling off both open that circuit and let the pullup take
  // over ("triggered" reads HIGH). See docs/04_Wiring_Electrical.md.
  pinMode(PIN_LIMIT_HOME, INPUT_PULLUP);
  pinMode(PIN_LIMIT_FAR, INPUT_PULLUP);

  pinMode(PIN_BUTTON_MODE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_START, INPUT_PULLUP);

  pinMode(PIN_LED_LOW, OUTPUT);
  pinMode(PIN_LED_NOMINAL, OUTPUT);
  pinMode(PIN_LED_MAX, OUTPUT);
  pinMode(PIN_LED_HEARTBEAT, OUTPUT);

#if ENCODER_ENABLED
  pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), encoderISR, RISING);
  Serial.println(F("Encoder: ENABLED (closed-loop speed control)"));
#else
  Serial.println(F("Encoder: DISABLED (open-loop, calibrated-PWM speed control)"));
#endif

  stopMotor();
  updateModeLEDs();

  Serial.println(F("Homing: driving toward the HOME limit switch..."));
  state = STATE_HOMING;
  phaseStartMs = millis();
}

// =============================================================================
// MAIN LOOP -- runs continuously. Never use delay() in here: doing so would
// stop us from noticing a limit switch or the E-stop status while we wait.
// =============================================================================
void loop() {
  startEdgeThisLoop = debouncedFallingEdge(startButton);
  modeEdgeThisLoop = debouncedFallingEdge(modeButton);
  readButtons();
  heartbeat();

  switch (state) {
    case STATE_HOMING:    runHoming();    break;
    case STATE_IDLE:      runIdle();      break;
    case STATE_ACCEL:     runAccel();     break;
    case STATE_CRUISE:    runCruise();    break;
    case STATE_DECEL:     runDecel();     break;
    case STATE_END_PAUSE: runEndPause();  break;
    case STATE_FAULT:     runFault();     break;
  }

  printTelemetry();
}

// =============================================================================
// STATE HANDLERS
// =============================================================================

// --- HOMING: crawl toward the home switch so we know where we are ---------
void runHoming() {
  direction = DIR_TOWARD_HOME;
  setMotor(PWM_LOW, direction);

  if (limitTriggered(PIN_LIMIT_HOME)) {
    stopMotor();
    Serial.println(F("Homing complete. Carriage is at the HOME end."));
    state = STATE_IDLE;
  }

  // Safety: if we're crawling toward home but somehow trip the FAR switch
  // instead, the DIR wiring is almost certainly reversed. Stop and say so
  // rather than fighting it.
  if (limitTriggered(PIN_LIMIT_FAR)) {
    raiseFault(F("Hit the FAR switch while homing toward HOME. "
                 "Check that PIN_MOTOR_DIR polarity matches your wiring "
                 "(see docs/04_Wiring_Electrical.md)."));
  }
}

// --- IDLE: stopped, waiting for the operator -------------------------------
void runIdle() {
  stopMotor();
  if (running) {
    Serial.println(F("START pressed -- beginning run."));
    direction = DIR_TOWARD_FAR;
    beginAccelPhase();
  }
}

// --- ACCEL: linear PWM ramp from 0 up to the target duty cycle ------------
void beginAccelPhase() {
  currentTargetSpeedInS = targetSpeedForMode(speedMode);
  rampStartPWM = 0;
  rampTargetPWM = pwmForMode(speedMode);
  cruiseDurationMs = (unsigned long)((MEASURED_TRAVEL_IN / currentTargetSpeedInS) * 1000.0);
  speedIntegral = 0;
  phaseStartMs = millis();
  state = STATE_ACCEL;
}

void runAccel() {
  unsigned long elapsed = millis() - phaseStartMs;
  if (checkForUnexpectedLimit(F("during ACCEL"))) return;

  if (elapsed >= RAMP_TIME_MS) {
    commandedPWM = rampTargetPWM;
    setMotor(commandedPWM, direction);
    Serial.println(F("Entering measured 6 ft zone (CRUISE)."));
    lastSpeedSampleMs = millis();
    lastEncoderSampleCount = encoderCount;
    phaseStartMs = millis();
    state = STATE_CRUISE;
    return;
  }

  // Linear ramp: 0% at elapsed=0, 100% at elapsed=RAMP_TIME_MS
  commandedPWM = map(elapsed, 0, RAMP_TIME_MS, rampStartPWM, rampTargetPWM);
  setMotor(commandedPWM, direction);
}

// --- CRUISE: hold target speed across the measured 6 ft zone ---------------
void runCruise() {
  unsigned long elapsed = millis() - phaseStartMs;
  if (checkForUnexpectedLimit(F("during CRUISE"))) return;

#if ENCODER_ENABLED
  updateMeasuredSpeed();
  runSpeedController();
#else
  commandedPWM = rampTargetPWM;
  setMotor(commandedPWM, direction);
#endif

  if (elapsed >= cruiseDurationMs) {
    Serial.println(F("Leaving measured zone, decelerating."));
    rampStartPWM = commandedPWM;
    rampTargetPWM = 0;
    phaseStartMs = millis();
    state = STATE_DECEL;
  }
}

// --- DECEL: ramp down, then creep-and-wait for the limit switch -----------
void runDecel() {
  unsigned long elapsed = millis() - phaseStartMs;

  if (limitTriggered(destinationLimitPin())) {
    stopMotor();
    Serial.println(F("Reached end of travel."));
    phaseStartMs = millis();
    state = STATE_END_PAUSE;
    return;
  }

  if (elapsed < RAMP_TIME_MS) {
    commandedPWM = map(elapsed, 0, RAMP_TIME_MS, rampStartPWM, 0);
    commandedPWM = max(commandedPWM, PWM_MIN_MOVING); // don't stall mid-ramp
  } else if (elapsed < RAMP_TIME_MS + 3000UL) {
    // Ramp said we should be there by now but the switch hasn't tripped --
    // creep slowly the rest of the way rather than guessing. This covers
    // normal calibration slop; it should NOT normally take this long.
    commandedPWM = PWM_MIN_MOVING;
  } else {
    raiseFault(F("Expected limit switch never triggered within 3 s of the "
                 "planned stop time. Check switch wiring/mounting, or your "
                 "BUFFER_IN / speed settings may be too aggressive."));
    return;
  }
  setMotor(commandedPWM, direction);
}

// --- END_PAUSE: brief dwell, then reverse (or stop if told to) ------------
void runEndPause() {
  stopMotor();
  if (millis() - phaseStartMs < END_PAUSE_MS) return;

  if (!running) {
    Serial.println(F("STOP requested -- run complete, returning to IDLE."));
    state = STATE_IDLE;
    return;
  }

  direction = (direction == DIR_TOWARD_FAR) ? DIR_TOWARD_HOME : DIR_TOWARD_FAR;
  Serial.println(direction == DIR_TOWARD_FAR
                  ? F("Reversing: heading toward FAR end.")
                  : F("Reversing: heading toward HOME end."));
  beginAccelPhase();
}

// --- FAULT: latch everything off until the operator investigates ----------
void runFault() {
  stopMotor();
  // Slow all-LEDs blink means "fault" -- distinct from the heartbeat blink.
  bool on = (millis() / 300) % 2;
  digitalWrite(PIN_LED_LOW, on);
  digitalWrite(PIN_LED_NOMINAL, on);
  digitalWrite(PIN_LED_MAX, on);

  if (startEdgeThisLoop) {
    Serial.println(F("START pressed after fault -- re-homing."));
    faultLatched = false;
    running = false;
    state = STATE_HOMING;
    phaseStartMs = millis();
  }
}

void raiseFault(const __FlashStringHelper *msg) {
  stopMotor();
  running = false;
  faultLatched = true;
  state = STATE_FAULT;
  Serial.print(F("*** FAULT: "));
  Serial.println(msg);
  Serial.println(F("*** Press START to clear the fault and re-home."));
}

// Which switch is "ahead" of the carriage given its current direction --
// the one it's expected to reach at the end of this pass.
uint8_t destinationLimitPin() {
  return (direction == DIR_TOWARD_FAR) ? PIN_LIMIT_FAR : PIN_LIMIT_HOME;
}

// Returns true (and raises a fault) if the DESTINATION limit switch trips
// somewhere we did NOT expect it to -- this should physically never happen
// if BUFFER_IN and the ramp timing are set correctly, so treat it as a real
// problem, not routine. We deliberately only watch the destination switch,
// not the one behind us: the switch we just departed from (at the start of
// every ACCEL phase, including every direction reversal) is still
// legitimately triggered for a moment as the carriage pulls away from it,
// and checking it here would raise a false fault on every single reversal.
bool checkForUnexpectedLimit(const __FlashStringHelper *when) {
  if (limitTriggered(destinationLimitPin())) {
    stopMotor();
    Serial.print(F("*** FAULT: unexpected limit switch triggered "));
    Serial.println(when);
    Serial.println(F("*** This usually means BUFFER_IN is too small for this "
                      "speed, or RAMP_TIME_MS is too long. See "
                      "docs/06_Calibration_and_Tuning.md."));
    running = false;
    faultLatched = true;
    state = STATE_FAULT;
    return true;
  }
  return false;
}

// =============================================================================
// CLOSED-LOOP SPEED CONTROL (PI controller) -- only compiled in if you wired
// up the encoder. Runs only during CRUISE; the accel/decel ramps stay
// open-loop timed ramps on purpose, since they're short and their exact
// shape doesn't matter -- only the cruise speed needs to hit tolerance.
// =============================================================================
#if ENCODER_ENABLED
void updateMeasuredSpeed() {
  unsigned long now = millis();
  if (now - lastSpeedSampleMs < SPEED_SAMPLE_MS) return;

  noInterrupts();
  long count = encoderCount;
  interrupts();

  long deltaCounts = count - lastEncoderSampleCount;
  float deltaSec = (now - lastSpeedSampleMs) / 1000.0;

  measuredSpeedInS = abs(deltaCounts) / COUNTS_PER_INCH / deltaSec;

  lastEncoderSampleCount = count;
  lastSpeedSampleMs = now;
}

void runSpeedController() {
  float error = currentTargetSpeedInS - measuredSpeedInS;
  speedIntegral += error * (SPEED_SAMPLE_MS / 1000.0);
  speedIntegral = constrain(speedIntegral, -40, 40); // anti-windup clamp

  float correction = (SPEED_KP * error) + (SPEED_KI * speedIntegral);
  commandedPWM = constrain(rampTargetPWM + (int)correction, PWM_MIN_MOVING, 255);
  setMotor(commandedPWM, direction);
}

void encoderISR() {
  // Simple 1x quadrature decode: on each rising edge of channel A, channel
  // B's current level tells us which way the shaft is turning.
  if (digitalRead(PIN_ENCODER_B) == HIGH) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}
#endif

// =============================================================================
// LOW-LEVEL MOTOR / SWITCH / BUTTON HELPERS
// =============================================================================
void setMotor(int pwm, TravelDir dir) {
  pwm = constrain(pwm, 0, 255);
  digitalWrite(PIN_MOTOR_DIR, dir == DIR_TOWARD_FAR ? HIGH : LOW);
  analogWrite(PIN_MOTOR_PWM, pwm);
}

void stopMotor() {
  analogWrite(PIN_MOTOR_PWM, 0);
  commandedPWM = 0;
}

bool limitTriggered(uint8_t pin) {
  // NC switch + internal pullup: HIGH means "triggered" (either the
  // carriage really is at that end, or a wire came loose -- both cases we
  // want to stop, so we don't try to tell them apart here).
  return digitalRead(pin) == HIGH;
}

int pwmForMode(SpeedMode mode) {
  switch (mode) {
    case MODE_LOW:     return PWM_LOW;
    case MODE_MAX:     return PWM_MAX;
    default:           return PWM_NOMINAL;
  }
}

float targetSpeedForMode(SpeedMode mode) {
  switch (mode) {
    case MODE_LOW:     return SPEED_LOW_IN_S;
    case MODE_MAX:     return SPEED_MAX_IN_S;
    default:           return SPEED_NOMINAL_IN_S;
  }
}

void updateModeLEDs() {
  digitalWrite(PIN_LED_LOW,     speedMode == MODE_LOW);
  digitalWrite(PIN_LED_NOMINAL, speedMode == MODE_NOMINAL);
  digitalWrite(PIN_LED_MAX,     speedMode == MODE_MAX);
}

void heartbeat() {
  if (millis() - lastHeartbeatMs >= 500) {
    lastHeartbeatMs = millis();
    heartbeatOn = !heartbeatOn;
    if (state != STATE_FAULT) digitalWrite(PIN_LED_HEARTBEAT, heartbeatOn);
  }
}

// --- Debounced button reading ----------------------------------------------
bool debouncedFallingEdge(Button &b) {
  bool reading = digitalRead(b.pin);
  if (reading != b.lastReading) {
    b.lastChangeMs = millis();
    b.lastReading = reading;
  }
  bool edge = false;
  if ((millis() - b.lastChangeMs) > 30 && b.stableState != reading) {
    edge = (b.stableState == HIGH && reading == LOW); // active-low button press
    b.stableState = reading;
  }
  return edge;
}

void readButtons() {
  if (modeEdgeThisLoop && state == STATE_IDLE) {
    // Only allow changing speed mode while stopped, so a run always executes
    // at one consistent speed from start to finish.
    speedMode = (SpeedMode)((speedMode + 1) % 3);
    updateModeLEDs();
    Serial.print(F("Speed mode -> "));
    Serial.println(speedMode == MODE_LOW ? F("LOW") :
                    speedMode == MODE_NOMINAL ? F("NOMINAL") : F("MAX"));
  }

  if (startEdgeThisLoop && state != STATE_FAULT) {
    running = !running;
    Serial.println(running ? F("RUN requested.") : F("STOP requested."));
  }
}

// =============================================================================
// TELEMETRY -- printed to the Serial Monitor (115200 baud) so you can watch
// what the rig is doing, or copy/paste a session log for troubleshooting.
// =============================================================================
void printTelemetry() {
  if (millis() - lastTelemetryMs < TELEMETRY_INTERVAL_MS) return;
  lastTelemetryMs = millis();

  Serial.print(F("state="));
  Serial.print(stateName(state));
  Serial.print(F(" mode="));
  Serial.print(speedMode == MODE_LOW ? F("LOW") : speedMode == MODE_NOMINAL ? F("NOM") : F("MAX"));
  Serial.print(F(" dir="));
  Serial.print(direction == DIR_TOWARD_FAR ? F("FAR") : F("HOME"));
  Serial.print(F(" pwm="));
  Serial.print(commandedPWM);
  Serial.print(F(" targetIn/s="));
  Serial.print(currentTargetSpeedInS, 2);
#if ENCODER_ENABLED
  Serial.print(F(" measuredIn/s="));
  Serial.print(measuredSpeedInS, 2);
#endif
  Serial.print(F(" homeSw="));
  Serial.print(limitTriggered(PIN_LIMIT_HOME) ? F("TRIG") : F("open"));
  Serial.print(F(" farSw="));
  Serial.print(limitTriggered(PIN_LIMIT_FAR) ? F("TRIG") : F("open"));
  Serial.println();
}

const __FlashStringHelper *stateName(RigState s) {
  switch (s) {
    case STATE_HOMING:    return F("HOMING");
    case STATE_IDLE:      return F("IDLE");
    case STATE_ACCEL:     return F("ACCEL");
    case STATE_CRUISE:    return F("CRUISE");
    case STATE_DECEL:     return F("DECEL");
    case STATE_END_PAUSE: return F("END_PAUSE");
    case STATE_FAULT:     return F("FAULT");
    default:               return F("?");
  }
}
