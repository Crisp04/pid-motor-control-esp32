#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

class Logger {
public:
    Logger(HardwareSerial &s);
    void begin(unsigned long baud = 115200);
    // Print CSV header
    void header();
    // Print a CSV line: timestamp_ms, setpoint, measured, duty
    void logLine(unsigned long timestamp_ms, double setpoint, double measured, double duty);
private:
    HardwareSerial &serial_;
};

#endif // LOGGER_H
