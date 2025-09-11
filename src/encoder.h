#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <stdint.h>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint32_t cpr = 1024);
    void begin();
    int32_t getAndResetDelta();
    int64_t getCount();

private:
    uint8_t pinA_;
    uint8_t pinB_;
    volatile int64_t count_;
    volatile uint8_t lastState_;
    uint32_t cpr_;

    portMUX_TYPE lock_;

    // Static ISR functions
    static void IRAM_ATTR isrA();
    static void IRAM_ATTR isrB();

    // Singleton pointer to current instance
    static Encoder* instance_;
};

#endif // ENCODER_H
