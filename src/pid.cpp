#include "pid.h"
#include <algorithm>

PID::PID(double kp, double ki, double kd, double out_min, double out_max)
    : kp_(kp), ki_(ki), kd_(kd), integrator_(0.0), prev_measurement_(0.0), out_min_(out_min), out_max_(out_max) {}

void PID::setGains(double kp, double ki, double kd) {
    kp_ = kp; ki_ = ki; kd_ = kd;
}

void PID::setOutputLimits(double minv, double maxv) {
    out_min_ = minv; out_max_ = maxv;
}

void PID::reset() {
    integrator_ = 0.0;
    prev_measurement_ = 0.0;
}

double PID::update(double setpoint, double measurement, double dt) {
    if (dt <= 0.0) return 0.0;
    double error = setpoint - measurement;

    // Proportional
    double P = kp_ * error;

    // Integral (simple discrete integration)
    integrator_ += error * dt;
    double I = ki_ * integrator_;

    // Derivative on measurement to reduce setpoint kick
    double derivative = (measurement - prev_measurement_) / dt;
    double D = -kd_ * derivative; // negative because derivative of measurement

    double out = P + I + D;

    // Clamp with simple anti-windup by clamping output and backing off integrator when saturating
    if (out > out_max_) {
        out = out_max_;
        // prevent further integrator wind-up
        // remove the last integration step
        integrator_ -= error * dt;
    } else if (out < out_min_) {
        out = out_min_;
        integrator_ -= error * dt;
    }

    prev_measurement_ = measurement;
    return out;
}
