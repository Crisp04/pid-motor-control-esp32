#!/usr/bin/env python3
"""
Simple simulator for motor speed control.
Simulates a first-order motor (PT1) and a quadrature encoder.

Outputs CSV of time_ms,setpoint_rpm,measured_rpm,duty
Generates a PNG plot (requires matplotlib).
"""

import math, csv, argparse, os
import matplotlib.pyplot as plt
import numpy as np

# motor plant parameters
TIME_CONST = 0.05  # seconds (plant time constant)
MAX_RPM_AT_1 = 6000.0  # motor free speed scaled to duty=1.0

LOOP_HZ = 200.0
DT = 1.0 / LOOP_HZ
TOTAL_SEC = 5.0

# simple PID (same semantics as C++ PID)
class PID:
    def __init__(self, kp=0.005, ki=0.03, kd=0.0, out_min=0.0, out_max=1.0):
        self.kp=kp; self.ki=ki; self.kd=kd
        self.out_min=out_min; self.out_max=out_max
        self.integrator=0.0
        self.prev_meas=0.0
    def reset(self):
        self.integrator=0.0; self.prev_meas=0.0
    def update(self, setpoint, meas, dt):
        if dt<=0: return 0.0
        err = setpoint - meas
        P = self.kp * err
        self.integrator += err * dt
        I = self.ki * self.integrator
        derivative = (meas - self.prev_meas) / dt
        D = -self.kd * derivative
        out = P + I + D
        if out > self.out_max:
            out = self.out_max
            self.integrator -= err * dt
        elif out < self.out_min:
            out = self.out_min
            self.integrator -= err * dt
        self.prev_meas = meas
        return out

def step_sim(kp,ki,kd, step_time=0.5):
    pid = PID(kp,ki,kd)
    t=0.0
    motor_rpm=0.0
    data=[]
    setpoint_low=0.0
    setpoint_high=3000.0
    while t < TOTAL_SEC:
        setpoint = setpoint_high if t>=step_time else setpoint_low
        duty = pid.update(setpoint, motor_rpm, DT)
        # plant: first order low-pass toward MAX_RPM_AT_1 * duty
        target = MAX_RPM_AT_1 * duty
        motor_rpm += (target - motor_rpm) * (DT / (TIME_CONST + 1e-9))
        data.append((int(t*1000), setpoint, motor_rpm, duty))
        t += DT
    return data

def write_csv(data, fname):
    with open(fname,"w",newline='') as f:
        w = csv.writer(f)
        w.writerow(["time_ms","setpoint_rpm","measured_rpm","duty"])
        w.writerows(data)

def plot_csv(data, out_png="sim_plot.png"):
    time = [r[0]/1000.0 for r in data]
    sp = [r[1] for r in data]
    meas = [r[2] for r in data]
    plt.figure(figsize=(6,3))
    plt.plot(time, sp, label="setpoint (RPM)")
    plt.plot(time, meas, label="measured (RPM)")
    plt.xlabel("Time (s)")
    plt.ylabel("RPM")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(out_png, dpi=150)
    print("Wrote", out_png)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--kp", type=float, default=0.005)
    parser.add_argument("--ki", type=float, default=0.03)
    parser.add_argument("--kd", type=float, default=0.0)
    parser.add_argument("--out", default="sim_output.csv")
    parser.add_argument("--plot", default="sim_plot.png")
    parser.add_argument("--step-time", type=float, default=0.5)
    args = parser.parse_args()
    data = step_sim(args.kp, args.ki, args.kd, step_time=args.step_time)
    write_csv(data, args.out)
    plot_csv(data, args.plot)
    print("Wrote CSV:", args.out)

if __name__ == "__main__":
    main()
