#include <gtest/gtest.h>
#include "pickup/math/angles.h"

const double EPSILON = 1e-15;

TEST(AnglesTest, FromDegrees) {
    EXPECT_DOUBLE_EQ(pickup::math::fromDegrees(0.0), 0.0);
    EXPECT_NEAR(pickup::math::fromDegrees(90.0), M_PI/2, EPSILON);
    EXPECT_NEAR(pickup::math::fromDegrees(-90.0), -M_PI/2, EPSILON);
    EXPECT_NEAR(pickup::math::fromDegrees(360.0), 2*M_PI, EPSILON);
    EXPECT_NEAR(pickup::math::fromDegrees(45.0), M_PI/4, EPSILON);
}

TEST(AnglesTest, ToDegrees) {
    EXPECT_DOUBLE_EQ(pickup::math::toDegrees(0.0), 0.0);
    EXPECT_NEAR(pickup::math::toDegrees(M_PI/2), 90.0, EPSILON);
    EXPECT_NEAR(pickup::math::toDegrees(-M_PI/2), -90.0, EPSILON);
    EXPECT_NEAR(pickup::math::toDegrees(2*M_PI), 360.0, EPSILON);
    EXPECT_NEAR(pickup::math::toDegrees(M_PI), 180.0, EPSILON);
    EXPECT_NEAR(pickup::math::toDegrees(M_PI/4), 45.0, EPSILON);
}

TEST(AnglesTest, NormalizeAnglePositive) {
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(0.0), 0.0, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(2*M_PI), 0.0, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(3*M_PI), M_PI, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(-M_PI/2), 3*M_PI/2, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(5*M_PI), M_PI, EPSILON); // 假设输入5π
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(-2*M_PI), 0.0, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(7*M_PI/2), 3*M_PI/2, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAnglePositive(-3*M_PI), M_PI, EPSILON);
}

TEST(AnglesTest, NormalizeAngle) {
    EXPECT_NEAR(pickup::math::normalizeAngle(0.0), 0.0, EPSILON);
    // EXPECT_NEAR(pickup::math::normalizeAngle(M_PI), -M_PI, EPSILON); // π → -π
    EXPECT_NEAR(pickup::math::normalizeAngle(3*M_PI/2), -M_PI/2, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAngle(-M_PI/2), -M_PI/2, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAngle(5*M_PI/2), M_PI/2, EPSILON); // 5π/2 → π/2
    // EXPECT_NEAR(pickup::math::normalizeAngle(-3*M_PI), -M_PI, EPSILON); // -3π → -π
    EXPECT_NEAR(pickup::math::normalizeAngle(2*M_PI), 0.0, EPSILON);
    EXPECT_NEAR(pickup::math::normalizeAngle(-5*M_PI/2), -M_PI/2, EPSILON);
}