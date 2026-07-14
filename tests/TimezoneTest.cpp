#include <gtest/gtest.h>
#include <string>

#include "pickup/time/Timezone.h"

using namespace pickup::time;

TEST(TimezoneTest, UtcTimezone) {
  auto tz = Timezone::utc();
  EXPECT_EQ(tz.name(), "GMT");
  EXPECT_EQ(tz.offset().total(), 0);
  EXPECT_EQ(tz.daylight().total(), 0);
}

TEST(TimezoneTest, NamedTimezone) {
  Timezone tz("CST", Timespan::hours(8));
  EXPECT_EQ(tz.name(), "CST");
  EXPECT_EQ(tz.offset().hours(), 8);
  EXPECT_EQ(tz.daylight().total(), 0);
}

TEST(TimezoneTest, NamedWithDst) {
  Timezone tz("PDT", Timespan::hours(-7), Timespan::hours(1));
  EXPECT_EQ(tz.name(), "PDT");
  EXPECT_EQ(tz.offset().hours(), -7);
  EXPECT_EQ(tz.daylight().hours(), 1);
}

TEST(TimezoneTest, TotalOffset) {
  Timezone tz("CEST", Timespan::hours(1), Timespan::hours(1));
  EXPECT_EQ(tz.total().hours(), 2);
}

TEST(TimezoneTest, Comparison) {
  Timezone tz1("UTC", Timespan::zero());
  Timezone tz2("CST", Timespan::hours(8));
  EXPECT_NE(tz1, tz2);
  EXPECT_TRUE(tz1 < tz2);
  EXPECT_TRUE(tz2 > tz1);
}

TEST(TimezoneTest, Equality) {
  Timezone tz1("CST", Timespan::hours(8));
  Timezone tz2("CST", Timespan::hours(8));
  EXPECT_EQ(tz1, tz2);
}

TEST(TimezoneTest, ConvertUtcToLocal) {
  Timezone tz("CST", Timespan::hours(8));
  UtcTime utc(2024, 1, 15, 10, 0, 0);
  auto local = tz.convert(utc);
  EXPECT_EQ(local.hour(), 18);
}

TEST(TimezoneTest, ConvertLocalToUtc) {
  Timezone tz("CST", Timespan::hours(8));
  LocalTime local(2024, 1, 15, 18, 0, 0);
  auto utc = tz.convert(local);
  EXPECT_EQ(utc.hour(), 10);
}

TEST(TimezoneTest, ConvertWithDst) {
  Timezone tz("PDT", Timespan::hours(-8), Timespan::hours(1));
  UtcTime utc(2024, 6, 15, 12, 0, 0);
  auto local = tz.convert(utc);
  EXPECT_EQ(local.hour(), 5);
}

TEST(TimezoneTest, DefaultConstructor) {
  Timezone tz;
  EXPECT_FALSE(tz.name().empty());
}

TEST(TimezoneTest, Swap) {
  Timezone tz1("A", Timespan::hours(1));
  Timezone tz2("B", Timespan::hours(2));
  swap(tz1, tz2);
  EXPECT_EQ(tz1.name(), "B");
  EXPECT_EQ(tz2.name(), "A");
}
