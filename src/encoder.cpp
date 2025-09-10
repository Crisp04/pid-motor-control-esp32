#include "encoder.h"
#include <Arduino.h>

Encoder* Encoder::instanceA = nullptr;

Encoder::Encoder(int pinA, int pinB, uint32_t cpr) : pinA_(pinA), pinB_(pinB), count_(0), last_state_(0), cpr_(cpr) {}

void Encoder::begin() {
    pinMode(pinA_, INPUT_PULLUP);
    pinMode(pinB_, INPUT_PULLUP);
    last_state_ = (digitalRead(pinA_) << 1) | digitalRead(pinB_);
    instanceA = this;
    attachInterruptArg(digitalPinToInterrupt(pinA_), (void(*)(void*))isrA, nullptr, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(pinB_), (void(*)(void*))isrB, nullptr, CHANGE);
}

// Returns delta counts since last call and resets internal count (thread-safe-ish)
int32_t Encoder::getAndResetDelta() {
    noInterrupts();
    int64_t val = count_;
    count_ = 0;
    interrupts();
    return (int32_t)val;
}

int64_t Encoder::getCount() {
    int64_t v;
    noInterrupts();
    v = count_;
    interrupts();
    return v;
}

void IRAM_ATTR Encoder::isrA() {
    if (!instanceA) return;
    uint8_t a = digitalRead(instanceA->pinA_);
    uint8_t b = digitalRead(instanceA->pinB_);
    uint8_t state = (a << 1) | b;
    int8_t delta = 0;
    // quadrature state machine
    if (state == instanceA->last_state_) return;
    // clockwise transitions
    if ((instanceA->last_state_ == 0 && state == 1) ||
        (instanceA->last_state_ == 1 && state == 3) ||
        (instanceA->last_state_ == 3 && state == 2) ||
        (instanceA->last_state_ == 2 && state == 0)) {
        delta = 1;
    } else {
        delta = -1;
    }
    instanceA->count_ += delta;
    instanceA->last_state_ = state;
}

void IRAM_ATTR Encoder::isrB() {
    // same handler as A to keep simple (reads both pins)
    isrA();
}
