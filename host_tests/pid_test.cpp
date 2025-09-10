#include "gtest/gtest.h"
#include "../src/pid.h"

// Test: with Kp only the output equals Kp*(error)
TEST(PIDBasic, ProportionalOnly) {
    PID pid(2.0, 0.0, 0.0, -10.0, 10.0);
    pid.reset();
    double out = pid.update(100.0, 90.0, 0.1); // error = 10
    EXPECT_NEAR(out, 20.0, 1e-6);
}

// Test: integrator accumulates over time
TEST(PIDBasic, IntegralAccumulation) {
    PID pid(0.0, 1.0, 0.0, -100.0, 100.0);
    pid.reset();
    double out1 = pid.update(10.0, 0.0, 0.1); // error 10 => integral += 1.0
    double out2 = pid.update(10.0, 0.0, 0.1); // integral += 1.0 => total 2.0
    EXPECT_NEAR(out1, 10.0, 1e-6);
    EXPECT_NEAR(out2, 20.0, 1e-6);
}

// Test: output saturates and integrator anti-windup prevents further windup
TEST(PIDBasic, SaturationAntiWindup) {
    PID pid(10.0, 100.0, 0.0, -1.0, 1.0);
    pid.reset();
    // big error, output will saturate
    double out1 = pid.update(1000.0, 0.0, 0.1);
    EXPECT_LE(out1, 1.0 + 1e-6);
    // call again with same error; integrator shouldn't grow unbounded due to anti-windup
    double out2 = pid.update(1000.0, 0.0, 0.1);
    EXPECT_LE(out2, 1.0 + 1e-6);
    // not NaN
    EXPECT_FALSE(std::isnan(out2));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
