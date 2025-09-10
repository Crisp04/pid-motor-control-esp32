#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

class Encoder {
public:
    Encoder(int pinA, int pinB, uint32_t cpr = 1024);
    void begin();
    // Return and reset counts atomically
    int32_t getAndResetDelta();
    // Get absolute count (non-reset)
    int64_t getCount();
private:
    int pinA_, pinB_;
    volatile int64_t count_;
    volatile uint8_t last_state_;
    uint32_t cpr_;
    // ISR handlers are C functions calling into static methods
    static Encoder* instanceA;
    static void IRAM_ATTR isrA();
    static void IRAM_ATTR isrB();
};
#endif // ENCODER_H
