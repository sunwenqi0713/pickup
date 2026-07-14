#include <gtest/gtest.h>
#include <string>

#include "pickup/time/Timespan.h"

using namespace pickup::time;

TEST(TimespanExTest, DefaultConstructor) {
  Timespan ts;
  EXPECT_EQ(ts.total(), 0);
  EXPECT_EQ(ts.nanoseconds(), 0);
  EXPECT_EQ(ts.seconds(), 0);
}

TEST(TimespanExTest, FromNanoseconds) {
  Timespan ts(1000);
  EXPECT_EQ(ts.nanoseconds(), 1000);
}

TEST(TimespanExTest, FactorySeconds) {
  auto ts = Timespan::seconds(5);
  EXPECT_EQ(ts.seconds(), 5);
  EXPECT_EQ(ts.total(), 5LL * 1000000000);
}

TEST(TimespanExTest, FactoryMinutes) {
  auto ts = Timespan::minutes(2);
  EXPECT_EQ(ts.minutes(), 2);
  EXPECT_EQ(ts.seconds(), 120);
}

TEST(TimespanExTest, FactoryHours) {
  auto ts = Timespan::hours(1);
  EXPECT_EQ(ts.hours(), 1);
  EXPECT_EQ(ts.minutes(), 60);
}

TEST(TimespanExTest, FactoryDays) {
  auto ts = Timespan::days(1);
  EXPECT_EQ(ts.days(), 1);
  EXPECT_EQ(ts.hours(), 24);
}

TEST(TimespanExTest, FactoryMilliseconds) {
  auto ts = Timespan::milliseconds(1500);
  EXPECT_EQ(ts.milliseconds(), 1500);
  EXPECT_EQ(ts.seconds(), 1);
}

TEST(TimespanExTest, FactoryMicroseconds) {
  auto ts = Timespan::microseconds(1000000);
  EXPECT_EQ(ts.microseconds(), 1000000);
  EXPECT_EQ(ts.seconds(), 1);
}

TEST(TimespanExTest, FromChronoDuration) {
  Timespan ts(std::chrono::seconds(10));
  EXPECT_EQ(ts.seconds(), 10);
}

TEST(TimespanExTest, NegativeTimespan) {
  Timespan ts(-1000000000);
  EXPECT_EQ(ts.seconds(), -1);
  EXPECT_EQ(ts.nanoseconds(), -1000000000);
}

TEST(TimespanExTest, Addition) {
  auto ts1 = Timespan::seconds(10);
  auto ts2 = Timespan::seconds(20);
  EXPECT_EQ((ts1 + ts2).seconds(), 30);
}

TEST(TimespanExTest, Subtraction) {
  auto ts1 = Timespan::seconds(30);
  auto ts2 = Timespan::seconds(10);
  EXPECT_EQ((ts1 - ts2).seconds(), 20);
}

TEST(TimespanExTest, UnaryMinus) {
  auto ts = -Timespan::seconds(5);
  EXPECT_EQ(ts.seconds(), -5);
}

TEST(TimespanExTest, CompoundAddAssign) {
  auto ts = Timespan::seconds(10);
  ts += Timespan::seconds(5);
  EXPECT_EQ(ts.seconds(), 15);
}

TEST(TimespanExTest, CompoundSubAssign) {
  auto ts = Timespan::seconds(10);
  ts -= Timespan::seconds(3);
  EXPECT_EQ(ts.seconds(), 7);
}

TEST(TimespanExTest, ComparisonEqual) {
  EXPECT_EQ(Timespan::seconds(5), Timespan::seconds(5));
}

TEST(TimespanExTest, ComparisonNotEqual) {
  EXPECT_NE(Timespan::seconds(5), Timespan::seconds(6));
}

TEST(TimespanExTest, ComparisonLessThan) {
  EXPECT_LT(Timespan::seconds(3), Timespan::seconds(10));
}

TEST(TimespanExTest, ComparisonGreaterThan) {
  EXPECT_GT(Timespan::seconds(10), Timespan::seconds(3));
}

TEST(TimespanExTest, Zero) {
  auto ts = Timespan::zero();
  EXPECT_EQ(ts.total(), 0);
}

TEST(TimespanExTest, ToStringZero) {
  EXPECT_EQ(Timespan::zero().toString(), "0ns");
}

TEST(TimespanExTest, ToStringSeconds) {
  EXPECT_EQ(Timespan::seconds(5).toString(), "5s");
}

TEST(TimespanExTest, ToStringMinutesAndSeconds) {
  auto ts = Timespan::minutes(2) + Timespan::seconds(30);
  EXPECT_EQ(ts.toString(), "2m30s");
}

TEST(TimespanExTest, ToStringNegative) {
  auto ts = -Timespan::seconds(3);
  EXPECT_EQ(ts.toString(), "-3s");
}

TEST(TimespanExTest, ToStringComplex) {
  auto ts = Timespan::days(1) + Timespan::hours(2) + Timespan::minutes(3) +
            Timespan::seconds(4) + Timespan::milliseconds(567);
  EXPECT_EQ(ts.toString(), "1d2h3m4s567ms");
}

TEST(TimespanExTest, Swap) {
  auto ts1 = Timespan::seconds(10);
  auto ts2 = Timespan::seconds(20);
  swap(ts1, ts2);
  EXPECT_EQ(ts1.seconds(), 20);
  EXPECT_EQ(ts2.seconds(), 10);
}

TEST(TimespanExTest, ChronoConversion) {
  auto ts = Timespan::seconds(3);
  auto c = ts.chrono();
  EXPECT_EQ(c.count(), 3000000000LL);
}

TEST(TimespanExTest, AssignmentFromInt64) {
  Timespan ts;
  ts = 5000000000LL;
  EXPECT_EQ(ts.seconds(), 5);
}
