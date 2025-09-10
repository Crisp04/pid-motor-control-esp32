#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <stdint.h>

// Simple PWM motor driver for ESP32 using LEDC
class MotorDriver {
public:
    MotorDriver(int pwmPin, int ch = 0, int freq = 20000, int resolution_bits = 12);
    void begin();
    // duty: 0.0..1.0 (sign ignored — this demo is uni-directional)
    void setDuty(double duty);
private:
    int pwmPin_;
    int ch_;
    int freq_;
    int resolution_bits_;
    int max_duty_;
};

#endif // MOTOR_DRIVER_H
