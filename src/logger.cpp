#include "logger.h"

Logger::Logger(HardwareSerial &s) : serial_(s) {}

void Logger::begin(unsigned long baud) {
    serial_.begin(baud);
    // small delay to let serial come up
    delay(50);
}

void Logger::header() {
    serial_.println("time_ms,setpoint_rpm,measured_rpm,duty");
}

void Logger::logLine(unsigned long timestamp_ms, double setpoint, double measured, double duty) {
    // CSV, fixed precision
    serial_.print(timestamp_ms);
    serial_.print(",");
    serial_.print(setpoint, 2);
    serial_.print(",");
    serial_.print(measured, 2);
    serial_.print(",");
    serial_.println(duty, 4);
}
