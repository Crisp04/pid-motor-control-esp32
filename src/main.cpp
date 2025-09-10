#include <Arduino.h>
#include "pid.h"
#include "encoder.h"
#include "motor_driver.h"
#include "logger.h"

// === Configuration ===
// Change pins here as needed
#ifndef LOOP_HZ
#define LOOP_HZ 200
#endif

#ifndef ENCODER_CPR
#define ENCODER_CPR 1024
#endif

const int PIN_PWM = 18;        // PWM to motor driver (use proper motor driver!)
const int PIN_ENC_A = 34;      // encoder A (input-capable pin)
const int PIN_ENC_B = 35;      // encoder B (input-capable pin)
const int PIN_BUTTON = 0;      // on-board button for step change (active LOW)

const double COUNTS_PER_REV = ENCODER_CPR; // counts per revolution (after quadrature)
const double LOOP_DT = 1.0 / (double)LOOP_HZ; // seconds
const int SERIAL_BAUD = 115200;

Encoder encoder(PIN_ENC_A, PIN_ENC_B, (uint32_t)COUNTS_PER_REV);
MotorDriver motor(PIN_PWM, 0, 20000, 12);
Logger logger(Serial);

// Example initial PID gains (tweak these)
double KP = 0.005; // proportional
double KI = 0.03;  // integral
double KD = 0.0;   // derivative

PID pid(KP, KI, KD, 0.0, 1.0);

volatile double setpoint_rpm = 0.0;
double step_high_rpm = 3000.0;
double step_low_rpm = 0.0;

// helper to compute rpm from delta counts and dt
double countsToRpm(int32_t delta_counts, double dt_sec) {
    double revs = ((double)delta_counts) / COUNTS_PER_REV;
    return revs * (60.0 / dt_sec);
}

void setup() {
    delay(10);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    encoder.begin();
    motor.begin();
    logger.begin(SERIAL_BAUD);
    logger.header();
    pid.reset();
    // start with low setpoint
    setpoint_rpm = step_low_rpm;
    Serial.println("PID motor-speed demo started");
    Serial.print("Loop Hz: "); Serial.println(LOOP_HZ);
}

void loop() {
    static unsigned long last_time = millis();
    static unsigned long last_loop = millis();
    // use vTaskDelayUntil-like timing
    unsigned long target_delay_ms = (unsigned long)(1000.0 / LOOP_HZ);
    unsigned long now = millis();
    // detect button press to toggle step
    static bool last_button_state = HIGH;
    bool button_state = digitalRead(PIN_BUTTON);
    if (last_button_state == HIGH && button_state == LOW) {
        // falling edge -> toggle
        if (setpoint_rpm < 1.0) setpoint_rpm = step_high_rpm;
        else setpoint_rpm = step_low_rpm;
    }
    last_button_state = button_state;

    // also support serial command 's' to toggle
    if (Serial.available()) {
        char c = Serial.read();
        if (c == 's') {
            if (setpoint_rpm < 1.0) setpoint_rpm = step_high_rpm;
            else setpoint_rpm = step_low_rpm;
            pid.reset();
        }
    }

    // control loop timing
    static unsigned long last_control = 0;
    unsigned long elapsed_ms = now - last_control;
    if (elapsed_ms < target_delay_ms) {
        delay(target_delay_ms - elapsed_ms);
        now = millis();
        elapsed_ms = now - last_control;
    }
    last_control = now;

    // 1) read encoder counts since last loop
    int32_t delta_counts = encoder.getAndResetDelta();

    // 2) estimate speed (RPM)
    double measured_rpm = countsToRpm(delta_counts, elapsed_ms / 1000.0);

    // 3) PID update (output 0..1)
    double control = pid.update(setpoint_rpm, measured_rpm, elapsed_ms / 1000.0);

    // 4) apply to motor (PWM duty)
    motor.setDuty(control);

    // 5) log CSV
    logger.logLine(millis(), setpoint_rpm, measured_rpm, control);

    // small background tasks could run here
}
