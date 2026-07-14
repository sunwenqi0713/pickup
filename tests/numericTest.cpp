#include <gtest/gtest.h>

#include "pickup/math/numeric.hpp"

using namespace pickup::math;

TEST(NumericTest, EqualIntegral) {
  EXPECT_TRUE(Equal(5, 5));
  EXPECT_FALSE(Equal(5, 6));
}

TEST(NumericTest, EqualFloat) {
  EXPECT_TRUE(Equal(1.0f, 1.00000005f));
  EXPECT_FALSE(Equal(1.0f, 1.1f));
}

TEST(NumericTest, EqualDouble) {
  EXPECT_TRUE(Equal(1.0, 1.0000000005));
  EXPECT_FALSE(Equal(1.0, 1.01));
}

TEST(NumericTest, NearFloat) {
  EXPECT_TRUE(Near(1.0f, 1.0005f));
  EXPECT_FALSE(Near(1.0f, 1.1f));
}

TEST(NumericTest, EqualZeroIntegral) {
  EXPECT_TRUE(EqualZero(0));
  EXPECT_FALSE(EqualZero(1));
}

TEST(NumericTest, EqualZeroFloat) {
  // float overload tested via Equal function
  EXPECT_TRUE(Equal(0.0f, 0.0f));
  EXPECT_TRUE(Equal(1e-7f, 0.0f));
  EXPECT_FALSE(Equal(0.1f, 0.0f));
}

TEST(NumericTest, BetweenIntegral) {
  EXPECT_TRUE(Between(5, 1, 10));
  EXPECT_TRUE(Between(1, 1, 10));
  EXPECT_TRUE(Between(10, 1, 10));
  EXPECT_FALSE(Between(0, 1, 10));
  EXPECT_FALSE(Between(11, 1, 10));
}

TEST(NumericTest, BetweenFloat) {
  EXPECT_TRUE(Between(0.5f, 0.0f, 1.0f));
  EXPECT_TRUE(Between(0.0f, 0.0f, 1.0f));
  EXPECT_TRUE(Between(1.0f, 0.0f, 1.0f));
}

TEST(NumericTest, FitInClamps) {
  EXPECT_EQ(FitIn(5, 0, 10), 5);
  EXPECT_EQ(FitIn(-5, 0, 10), 0);
  EXPECT_EQ(FitIn(15, 0, 10), 10);
}

TEST(NumericTest, FitInThrowsOnInvalidRange) {
  EXPECT_THROW((void)FitIn(5, 10, 0), std::invalid_argument);
}

TEST(NumericTest, Sign) {
  EXPECT_EQ(Sign(5), 1);
  EXPECT_EQ(Sign(0), 1);
  EXPECT_EQ(Sign(-5), -1);
}

TEST(NumericTest, SignFloat) {
  EXPECT_EQ(Sign(3.14f), 1);
  EXPECT_EQ(Sign(-2.71f), -1);
  EXPECT_EQ(Sign(0.0f), 1);
}
