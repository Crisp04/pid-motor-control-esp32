#include "encoder.h"

Encoder* Encoder::instance_ = nullptr;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint32_t cpr)
    : pinA_(pinA), pinB_(pinB), count_(0), lastState_(0), cpr_(cpr),
      lock_(portMUX_INITIALIZER_UNLOCKED)
{}

void Encoder::begin() {
    // store singleton instance
    instance_ = this;

    pinMode(pinA_, INPUT_PULLUP);
    pinMode(pinB_, INPUT_PULLUP);

    // read initial state
    uint8_t a = digitalRead(pinA_);
    uint8_t b = digitalRead(pinB_);
    lastState_ = (a << 1) | b;

    // attach static ISR
    attachInterrupt(digitalPinToInterrupt(pinA_), Encoder::isrA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pinB_), Encoder::isrB, CHANGE);
}

int32_t Encoder::getAndResetDelta() {
    portENTER_CRITICAL(&lock_);
    int64_t val = count_;
    count_ = 0;
    portEXIT_CRITICAL(&lock_);
    return (int32_t)val;
}

int64_t Encoder::getCount() {
    portENTER_CRITICAL(&lock_);
    int64_t val = count_;
    portEXIT_CRITICAL(&lock_);
    return val;
}

// ===================
// Static ISR functions
// ===================
void IRAM_ATTR Encoder::isrA() {
    if (!instance_) return;

    uint8_t a = digitalRead(instance_->pinA_);
    uint8_t b = digitalRead(instance_->pinB_);
    uint8_t state = (a << 1) | b;

    if (state == instance_->lastState_) return;

    int8_t delta = 0;
    // CW transitions
    if ((instance_->lastState_ == 0 && state == 1) ||
        (instance_->lastState_ == 1 && state == 3) ||
        (instance_->lastState_ == 3 && state == 2) ||
        (instance_->lastState_ == 2 && state == 0)) {
        delta = 1;
    } else {
        delta = -1;
    }

    portENTER_CRITICAL_ISR(&instance_->lock_);
    instance_->count_ += delta;
    instance_->lastState_ = state;
    portEXIT_CRITICAL_ISR(&instance_->lock_);
}

void IRAM_ATTR Encoder::isrB() {
    // same as isrA (could also call a common handler)
    if (!instance_) return;

    uint8_t a = digitalRead(instance_->pinA_);
    uint8_t b = digitalRead(instance_->pinB_);
    uint8_t state = (a << 1) | b;

    if (state == instance_->lastState_) return;

    int8_t delta = 0;
    // CW transitions
    if ((instance_->lastState_ == 0 && state == 1) ||
        (instance_->lastState_ == 1 && state == 3) ||
        (instance_->lastState_ == 3 && state == 2) ||
        (instance_->lastState_ == 2 && state == 0)) {
        delta = 1;
    } else {
        delta = -1;
    }

    portENTER_CRITICAL_ISR(&instance_->lock_);
    instance_->count_ += delta;
    instance_->lastState_ = state;
    portEXIT_CRITICAL_ISR(&instance_->lock_);
}
