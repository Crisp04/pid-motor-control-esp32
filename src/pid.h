#ifndef PID_H
#define PID_H

#include <stdint.h>

class PID {
public:
    PID(double kp=0.1, double ki=0.0, double kd=0.0, double out_min=0.0, double out_max=1.0);

    // Set gains
    void setGains(double kp, double ki, double kd);
    // Compute control (setpoint and measurement in same units). dt in seconds.
    double update(double setpoint, double measurement, double dt);
    // Reset integrator/derivative
    void reset();

    // Tweak limits
    void setOutputLimits(double minv, double maxv);

private:
    double kp_, ki_, kd_;
    double integrator_;
    double prev_measurement_;
    double out_min_, out_max_;
};
#endif // PID_H
