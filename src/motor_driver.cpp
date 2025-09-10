#include "motor_driver.h"
#include <Arduino.h>

MotorDriver::MotorDriver(int pwmPin, int ch, int freq, int resolution_bits)
    : pwmPin_(pwmPin), ch_(ch), freq_(freq), resolution_bits_(resolution_bits) {
    max_duty_ = (1 << resolution_bits_) - 1;
}

void MotorDriver::begin() {
    ledcSetup(ch_, freq_, resolution_bits_);
    ledcAttachPin(pwmPin_, ch_);
}

void MotorDriver::setDuty(double duty) {
    if (duty < 0.0) duty = 0.0;
    if (duty > 1.0) duty = 1.0;
    uint32_t val = (uint32_t)(duty * max_duty_);
    ledcWrite(ch_, val);
}
