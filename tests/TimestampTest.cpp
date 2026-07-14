#include <gtest/gtest.h>
#include <chrono>

#include "pickup/time/Timestamp.h"

using namespace pickup::time;

TEST(TimestampExTest, DefaultConstructor) {
  Timestamp ts;
  EXPECT_EQ(ts.total(), 0);
  EXPECT_EQ(ts.seconds(), 0);
}

TEST(TimestampExTest, FromNanoseconds) {
  Timestamp ts(1000000000);
  EXPECT_EQ(ts.seconds(), 1);
}

TEST(TimestampExTest, FactorySeconds) {
  auto ts = Timestamp::seconds(100);
  EXPECT_EQ(ts.seconds(), 100);
}

TEST(TimestampExTest, FactoryDays) {
  auto ts = Timestamp::days(1);
  EXPECT_EQ(ts.days(), 1);
  EXPECT_EQ(ts.hours(), 24);
}

TEST(TimestampExTest, FromChronoTimePoint) {
  auto now = std::chrono::system_clock::now();
  Timestamp ts(now);
  auto diff = ts.total() - std::chrono::duration_cast<std::chrono::nanoseconds>(
                                now.time_since_epoch()).count();
  EXPECT_LT(diff, 1000000);
}

TEST(TimestampExTest, Addition) {
  auto ts = Timestamp::seconds(100);
  auto result = ts + 5000000000LL;
  EXPECT_EQ(result.seconds(), 105);
}

TEST(TimestampExTest, AdditionWithTimespan) {
  auto ts = Timestamp::seconds(100);
  auto span = Timespan::seconds(50);
  auto result = ts + span;
  EXPECT_EQ(result.seconds(), 150);
}

TEST(TimestampExTest, Subtraction) {
  auto ts1 = Timestamp::seconds(100);
  auto ts2 = Timestamp::seconds(30);
  auto diff = ts1 - ts2;
  EXPECT_EQ(diff.seconds(), 70);
}

TEST(TimestampExTest, Comparison) {
  auto ts1 = Timestamp::seconds(100);
  auto ts2 = Timestamp::seconds(200);
  EXPECT_TRUE(ts1 < ts2);
  EXPECT_TRUE(ts2 > ts1);
  EXPECT_TRUE(ts1 != ts2);
  EXPECT_TRUE(ts1 == Timestamp::seconds(100));
}

TEST(TimestampExTest, CompoundAddAssign) {
  auto ts = Timestamp::seconds(50);
  ts += Timespan::seconds(25);
  EXPECT_EQ(ts.seconds(), 75);
}

TEST(TimestampExTest, CompoundSubAssign) {
  auto ts = Timestamp::seconds(50);
  ts -= Timespan::seconds(15);
  EXPECT_EQ(ts.seconds(), 35);
}

TEST(TimestampExTest, ChronoConversion) {
  auto ts = Timestamp::seconds(100);
  auto cp = ts.chrono();
  auto sec = std::chrono::duration_cast<std::chrono::seconds>(cp.time_since_epoch()).count();
  EXPECT_EQ(sec, 100);
}

TEST(TimestampExTest, Epoch) {
  EXPECT_EQ(Timestamp::epoch(), 0);
}

TEST(TimestampExTest, UtcReturnsPositive) {
  auto now = Timestamp::utc();
  EXPECT_GT(now, 0);
}

TEST(TimestampExTest, NanoMonotonic) {
  auto t1 = Timestamp::nano();
  auto t2 = Timestamp::nano();
  EXPECT_GE(t2, t1);
}

TEST(TimestampExTest, UtcTimestampConvenience) {
  UtcTimestamp uts;
  EXPECT_GT(uts.total(), 0);
}

TEST(TimestampExTest, LocalTimestampConvenience) {
  LocalTimestamp lts;
  EXPECT_GT(lts.total(), 0);
}

TEST(TimestampExTest, Swap) {
  auto ts1 = Timestamp::seconds(10);
  auto ts2 = Timestamp::seconds(20);
  swap(ts1, ts2);
  EXPECT_EQ(ts1.seconds(), 20);
  EXPECT_EQ(ts2.seconds(), 10);
}

TEST(TimestampExTest, AssignmentFromInt64) {
  Timestamp ts;
  ts = 5000000000LL;
  EXPECT_EQ(ts.seconds(), 5);
}

TEST(TimestampExTest, Milliseconds) {
  auto ts = Timestamp::milliseconds(2500);
  EXPECT_EQ(ts.milliseconds(), 2500);
  EXPECT_EQ(ts.seconds(), 2);
}

TEST(TimestampExTest, Microseconds) {
  auto ts = Timestamp::microseconds(2000000);
  EXPECT_EQ(ts.microseconds(), 2000000);
  EXPECT_EQ(ts.seconds(), 2);
}

TEST(TimestampExTest, Nanoseconds) {
  auto ts = Timestamp::nanoseconds(3000000000LL);
  EXPECT_EQ(ts.nanoseconds(), 3000000000LL);
  EXPECT_EQ(ts.seconds(), 3);
}
