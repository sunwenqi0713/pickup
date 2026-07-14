#include <gtest/gtest.h>
#include <chrono>
#include <string>

#include "pickup/time/Time.h"
#include "pickup/time/Timespan.h"
#include "pickup/time/Timestamp.h"

using namespace pickup::time;

// ============================================================
// Time construction
// ============================================================

TEST(TimeTest, DefaultConstructor) {
  Time t;
  EXPECT_EQ(t.year(), 1970);
  EXPECT_EQ(t.month(), 1);
  EXPECT_EQ(t.day(), 1);
  EXPECT_EQ(t.hour(), 0);
  EXPECT_EQ(t.minute(), 0);
  EXPECT_EQ(t.second(), 0);
  EXPECT_EQ(t.millisecond(), 0);
}

TEST(TimeTest, FieldConstructor) {
  Time t(2025, 3, 14, 10, 1, 17);
  EXPECT_EQ(t.year(), 2025);
  EXPECT_EQ(t.month(), 3);
  EXPECT_EQ(t.day(), 14);
  EXPECT_EQ(t.hour(), 10);
  EXPECT_EQ(t.minute(), 1);
  EXPECT_EQ(t.second(), 17);
  EXPECT_EQ(t.millisecond(), 0);
}

TEST(TimeTest, FieldConstructorWithMs) {
  Time t(2024, 12, 25, 8, 30, 15, 500);
  EXPECT_EQ(t.year(), 2024);
  EXPECT_EQ(t.month(), 12);
  EXPECT_EQ(t.day(), 25);
  EXPECT_EQ(t.millisecond(), 500);
}

TEST(TimeTest, FieldConstructorSubsecond) {
  Time t(2024, 1, 1, 0, 0, 0, 123, 456, 789);
  EXPECT_EQ(t.millisecond(), 123);
  EXPECT_EQ(t.microsecond(), 456);
  EXPECT_EQ(t.nanosecond(), 789);
}

TEST(TimeTest, Epoch) {
  Time t = Time::epoch();
  EXPECT_EQ(t.year(), 1970);
  EXPECT_EQ(t.month(), 1);
  EXPECT_EQ(t.day(), 1);
}

TEST(TimeTest, FieldOutOfRangeThrows) {
  EXPECT_THROW(Time(3000, 1, 1), std::invalid_argument);
  EXPECT_THROW(Time(1970, 13, 1), std::invalid_argument);
  EXPECT_THROW(Time(1970, 1, 32), std::invalid_argument);
  EXPECT_THROW(Time(1970, 1, 1, 24, 0, 0), std::invalid_argument);
  EXPECT_THROW(Time(1970, 1, 1, 0, 60, 0), std::invalid_argument);
  EXPECT_THROW(Time(1970, 1, 1, 0, 0, 60), std::invalid_argument);
  EXPECT_THROW(Time(1970, 1, 1, 0, 0, 0, 1000), std::invalid_argument);
  EXPECT_THROW(Time(1970, 1, 1, 0, 0, 0, 0, 1000), std::invalid_argument);
  EXPECT_THROW(Time(1970, 1, 1, 0, 0, 0, 0, 0, 1000), std::invalid_argument);
}

// ============================================================
// Weekday
// ============================================================

TEST(TimeTest, Weekday) {
  // 2025-03-15 是周六 (Saturday = 6)
  Time t(2025, 3, 15);
  EXPECT_EQ(t.weekday(), Weekday::Saturday);
  // 2025-03-17 是周一 (Monday = 1)
  Time t2(2025, 3, 17);
  EXPECT_EQ(t2.weekday(), Weekday::Monday);
}

// ============================================================
// toISO8601
// ============================================================

TEST(TimeTest, ToISO8601UtcExtended) {
  UtcTime t(2024, 1, 15, 10, 30, 0, 123);
  std::string iso = t.toISO8601(true);
  EXPECT_EQ(iso, "2024-01-15T10:30:00.123Z");
}

TEST(TimeTest, ToISO8601UtcCompact) {
  UtcTime t(2024, 1, 15, 10, 30, 0, 123);
  std::string iso = t.toISO8601(false);
  EXPECT_EQ(iso, "20240115T103000.123Z");
}

TEST(TimeTest, ToISO8601UtcNoMs) {
  UtcTime t(2024, 1, 15, 10, 30, 0);
  std::string iso = t.toISO8601(true);
  EXPECT_EQ(iso, "2024-01-15T10:30:00.000Z");
}

// ============================================================
// formatted (strftime)
// ============================================================

TEST(TimeTest, FormattedStrftime) {
  Time t(2024, 3, 14, 10, 1, 17);
  std::string result = t.formatted("%Y-%m-%d %H:%M:%S");
  EXPECT_EQ(result, "2024-03-14 10:01:17");
}

TEST(TimeTest, FormattedSubsecond3f) {
  Time t(2024, 1, 1, 0, 0, 0, 7);
  std::string result = t.formatted("%3f");
  EXPECT_EQ(result, "007");
}

TEST(TimeTest, FormattedSubsecond6f) {
  Time t(2024, 1, 1, 0, 0, 0, 1, 2);
  std::string result = t.formatted("%6f");
  EXPECT_EQ(result, "001002");
}

TEST(TimeTest, FormattedSubsecond9f) {
  Time t(2024, 1, 1, 0, 0, 0, 0, 0, 7);
  std::string result = t.formatted("%9f");
  EXPECT_EQ(result, "000000007");
}

TEST(TimeTest, FormattedLiteralPercent) {
  Time t(2024, 1, 1);
  std::string result = t.formatted("%%Y");
  EXPECT_EQ(result, "%Y");
}

TEST(TimeTest, FormattedCombined) {
  Time t(2024, 3, 14, 10, 1, 17, 42);
  std::string result = t.formatted("%Y-%m-%d %H:%M:%S.%3f");
  EXPECT_EQ(result, "2024-03-14 10:01:17.042");
}

// ============================================================
// toString
// ============================================================

TEST(TimeTest, ToStringDateAndTime24h) {
  Time t(2024, 3, 14, 10, 1, 17);
  std::string result = t.toString(true, true, true, true);
  EXPECT_EQ(result, "14 Mar 2024 10:01:17");
}

TEST(TimeTest, ToStringDateOnly) {
  Time t(2024, 3, 14);
  std::string result = t.toString(true, false);
  EXPECT_EQ(result, "14 Mar 2024");
}

TEST(TimeTest, ToStringTimeOnly24h) {
  Time t(2024, 3, 14, 10, 1, 17);
  std::string result = t.toString(false, true, true, true);
  EXPECT_EQ(result, "10:01:17");
}

TEST(TimeTest, ToStringTime12hAm) {
  Time t(2024, 3, 14, 8, 30, 0);
  std::string result = t.toString(false, true, true, false);
  EXPECT_EQ(result, "08:30:00am");
}

TEST(TimeTest, ToStringTime12hPm) {
  Time t(2024, 3, 14, 20, 30, 0);
  std::string result = t.toString(false, true, true, false);
  EXPECT_EQ(result, "08:30:00pm");
}

TEST(TimeTest, ToStringNoSeconds) {
  Time t(2024, 3, 14, 10, 1, 17);
  std::string result = t.toString(true, true, false, true);
  EXPECT_EQ(result, "14 Mar 2024 10:01");
}

// ============================================================
// Static helper methods
// ============================================================

TEST(TimeTest, GetMonthNameAbbreviated) {
  EXPECT_EQ(Time::getMonthName(1, true), "Jan");
  EXPECT_EQ(Time::getMonthName(12, true), "Dec");
}

TEST(TimeTest, GetMonthNameFull) {
  EXPECT_EQ(Time::getMonthName(1, false), "January");
  EXPECT_EQ(Time::getMonthName(12, false), "December");
}

TEST(TimeTest, GetWeekdayNameAbbreviated) {
  EXPECT_EQ(Time::getWeekdayName(0, true), "Sun");
  EXPECT_EQ(Time::getWeekdayName(6, true), "Sat");
}

TEST(TimeTest, GetWeekdayNameFull) {
  EXPECT_EQ(Time::getWeekdayName(0, false), "Sunday");
  EXPECT_EQ(Time::getWeekdayName(6, false), "Saturday");
}

// ============================================================
// fromISO8601
// ============================================================

TEST(TimeTest, FromISO8601ExtendedUtc) {
  auto t = Time::fromISO8601("2024-01-15T10:30:00Z");
  EXPECT_EQ(t.year(), 2024);
  EXPECT_EQ(t.month(), 1);
  EXPECT_EQ(t.day(), 15);
  EXPECT_EQ(t.hour(), 10);
  EXPECT_EQ(t.minute(), 30);
  EXPECT_EQ(t.second(), 0);
}

TEST(TimeTest, FromISO8601CompactUtc) {
  auto t = Time::fromISO8601("20240115T103000Z");
  EXPECT_EQ(t.year(), 2024);
  EXPECT_EQ(t.month(), 1);
  EXPECT_EQ(t.day(), 15);
  EXPECT_EQ(t.hour(), 10);
  EXPECT_EQ(t.minute(), 30);
}

TEST(TimeTest, FromISO8601WithMs) {
  auto t = Time::fromISO8601("2024-01-15T10:30:00.123Z");
  EXPECT_EQ(t.millisecond(), 123);
}

TEST(TimeTest, FromISO8601WithTzOffset) {
  auto t = Time::fromISO8601("2024-01-15T10:30:00+08:00");
  // +08:00 偏移，UTC 时间为 02:30
  EXPECT_EQ(t.hour(), 2);
  EXPECT_EQ(t.minute(), 30);
}

TEST(TimeTest, FromISO8601CompactTz) {
  auto t = Time::fromISO8601("20240115T103000+0800");
  EXPECT_EQ(t.hour(), 2);
  EXPECT_EQ(t.minute(), 30);
}

TEST(TimeTest, FromISO8601NoTz) {
  // 缺省时区视为 UTC
  auto t = Time::fromISO8601("2024-01-15T10:30:00");
  EXPECT_EQ(t.hour(), 10);
}

TEST(TimeTest, FromISO8601InvalidReturnsEpoch) {
  auto t = Time::fromISO8601("not-a-date");
  EXPECT_EQ(t.year(), 1970);
  EXPECT_EQ(t.month(), 1);
  EXPECT_EQ(t.day(), 1);
}

// ============================================================
// Comparison operators
// ============================================================

TEST(TimeTest, Equality) {
  Time a(2024, 1, 1, 10, 0, 0);
  Time b(2024, 1, 1, 10, 0, 0);
  Time c(2024, 1, 1, 10, 0, 1);
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
  EXPECT_TRUE(a != c);
}

TEST(TimeTest, LessThan) {
  Time a(2024, 1, 1);
  Time b(2024, 1, 2);
  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);
}

TEST(TimeTest, GreaterThan) {
  Time a(2024, 1, 2);
  Time b(2024, 1, 1);
  EXPECT_TRUE(a > b);
  EXPECT_FALSE(b > a);
}

TEST(TimeTest, LessThanOrEqual) {
  Time a(2024, 1, 1);
  Time b(2024, 1, 1);
  Time c(2024, 1, 2);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(a <= c);
  EXPECT_FALSE(c <= a);
}

TEST(TimeTest, GreaterThanOrEqual) {
  Time a(2024, 1, 2);
  Time b(2024, 1, 2);
  Time c(2024, 1, 1);
  EXPECT_TRUE(a >= b);
  EXPECT_TRUE(a >= c);
  EXPECT_FALSE(c >= a);
}

// ============================================================
// Arithmetic
// ============================================================

TEST(TimeTest, AddTimespan) {
  UtcTime t(2024, 1, 1, 10, 0, 0);
  UtcTime result = t + Timespan::hours(2);
  EXPECT_EQ(result.hour(), 12);
}

TEST(TimeTest, SubtractTimespan) {
  UtcTime t(2024, 1, 1, 10, 0, 0);
  UtcTime result = t - Timespan::hours(3);
  EXPECT_EQ(result.hour(), 7);
}

TEST(TimeTest, Difference) {
  UtcTime a(2024, 1, 1, 10, 0, 0);
  UtcTime b(2024, 1, 1, 12, 30, 0);
  Timespan diff = b - a;
  EXPECT_EQ(diff.hours(), 2);
  EXPECT_EQ(diff.minutes(), 150);
}

TEST(TimeTest, CompoundAdd) {
  UtcTime t(2024, 1, 1, 10, 0, 0);
  t += Timespan::minutes(30);
  EXPECT_EQ(t.minute(), 30);
}

TEST(TimeTest, CompoundSubtract) {
  UtcTime t(2024, 1, 1, 10, 0, 0);
  t -= Timespan::minutes(15);
  EXPECT_EQ(t.minute(), 45);  // 09:45
  EXPECT_EQ(t.hour(), 9);
}

// ============================================================
// UtcTime
// ============================================================

TEST(UtcTimeTest, DefaultConstructorReturnsNow) {
  UtcTime t;
  // 默认构造为当前 UTC 时刻，年份应合理（>= 2024）
  EXPECT_GE(t.year(), 2024);
  EXPECT_LE(t.year(), 2030);
}

TEST(UtcTimeTest, FromTimestamp) {
  Timestamp ts(Timestamp::hours(48));  // 1970-01-03 00:00:00 UTC
  UtcTime t(ts);
  EXPECT_EQ(t.day(), 3);
}

TEST(UtcTimeTest, ToISO8601) {
  UtcTime t(2024, 6, 15, 12, 0, 0);
  EXPECT_EQ(t.toISO8601(true), "2024-06-15T12:00:00.000Z");
}

TEST(UtcTimeTest, ChronoConversion) {
  UtcTime t(2024, 1, 1);
  auto tp = t.chrono();
  UtcTime t2(tp);
  EXPECT_EQ(t, t2);
}

// ============================================================
// LocalTime
// ============================================================

TEST(LocalTimeTest, DefaultConstructorReturnsNow) {
  LocalTime t;
  // 默认构造为当前本地时刻，年份应合理
  EXPECT_GE(t.year(), 2024);
  EXPECT_LE(t.year(), 2030);
}

TEST(LocalTimeTest, ToISO8601Offset) {
  LocalTime t(2024, 1, 15, 10, 30, 0);
  std::string iso = t.toISO8601(true);
  // 包含本地时区偏移，如 "+08:00" 或 "-05:00"
  EXPECT_NE(iso.back(), 'Z');  // 不是 UTC
  EXPECT_TRUE(iso.find('+') != std::string::npos || iso.find('-') != std::string::npos);
}

// ============================================================
// UtcTime <-> LocalTime conversion
// ============================================================

TEST(TimeConversionTest, UtcToLocal) {
  UtcTime utc(2024, 6, 15, 12, 0, 0);
  LocalTime local(utc);
  // 本地时间应不同于 UTC（除非所在时区与 UTC 相同或恰好对齐）
  // 本地偏移不可能超过 ±12h，所以本地小时应在 [0, 23]
  EXPECT_GE(local.hour(), 0);
  EXPECT_LE(local.hour(), 23);
}

TEST(TimeConversionTest, LocalToUtc) {
  LocalTime local(2024, 6, 15, 12, 0, 0);
  UtcTime utc(local);
  EXPECT_GE(utc.hour(), 0);
  EXPECT_LE(utc.hour(), 23);
}

// ============================================================
// Swap
// ============================================================

TEST(TimeTest, Swap) {
  Time a(2024, 1, 1);
  Time b(2025, 12, 31);
  a.swap(b);
  EXPECT_EQ(a.year(), 2025);
  EXPECT_EQ(b.year(), 2024);
}

// ============================================================
// Timespan
// ============================================================

TEST(TimespanTest, Zero) {
  Timespan ts;
  EXPECT_EQ(ts.total(), 0);
  EXPECT_EQ(ts.toString(), "0ns");
}

TEST(TimespanTest, FactorySeconds) {
  Timespan ts = Timespan::seconds(5);
  EXPECT_EQ(ts.seconds(), 5);
  EXPECT_EQ(ts.milliseconds(), 5000);
}

TEST(TimespanTest, FactoryMilliseconds) {
  Timespan ts = Timespan::milliseconds(1500);
  EXPECT_EQ(ts.seconds(), 1);
  EXPECT_EQ(ts.milliseconds(), 1500);
}

TEST(TimespanTest, FactoryMinutes) {
  Timespan ts = Timespan::minutes(2);
  EXPECT_EQ(ts.seconds(), 120);
}

TEST(TimespanTest, FactoryHours) {
  Timespan ts = Timespan::hours(1);
  EXPECT_EQ(ts.minutes(), 60);
}

TEST(TimespanTest, FactoryDays) {
  Timespan ts = Timespan::days(1);
  EXPECT_EQ(ts.hours(), 24);
}

TEST(TimespanTest, Negation) {
  Timespan ts = Timespan::seconds(5);
  Timespan neg = -ts;
  EXPECT_EQ(neg.seconds(), -5);
}

TEST(TimespanTest, Addition) {
  Timespan a = Timespan::minutes(1);
  Timespan b = Timespan::seconds(30);
  Timespan sum = a + b;
  EXPECT_EQ(sum.seconds(), 90);
}

TEST(TimespanTest, Subtraction) {
  Timespan a = Timespan::minutes(1);
  Timespan b = Timespan::seconds(15);
  Timespan diff = a - b;
  EXPECT_EQ(diff.seconds(), 45);
}

TEST(TimespanTest, CompoundAdd) {
  Timespan ts = Timespan::seconds(10);
  ts += Timespan::seconds(5);
  EXPECT_EQ(ts.seconds(), 15);
}

TEST(TimespanTest, Comparison) {
  Timespan a = Timespan::seconds(10);
  Timespan b = Timespan::seconds(20);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(a == Timespan::seconds(10));
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(a >= Timespan::seconds(5));
}

TEST(TimespanTest, ToString) {
  EXPECT_EQ(Timespan::days(1).toString(), "1d");
  EXPECT_EQ(Timespan::hours(2).toString(), "2h");
  EXPECT_EQ(Timespan::minutes(3).toString(), "3m");
  EXPECT_EQ(Timespan::seconds(4).toString(), "4s");
  EXPECT_EQ(Timespan::milliseconds(567).toString(), "567ms");
  EXPECT_EQ(Timespan::microseconds(890).toString(), "890us");
  EXPECT_EQ(Timespan::nanoseconds(1).toString(), "1ns");
  EXPECT_EQ((Timespan::days(1) + Timespan::hours(2)).toString(), "1d2h");
  EXPECT_EQ(Timespan(-1500).toString(), "-1us500ns");
}

// ============================================================
// Timestamp
// ============================================================

TEST(TimestampTest, DefaultConstructor) {
  Timestamp ts;
  EXPECT_EQ(ts.total(), 0);
}

TEST(TimestampTest, FactoryHours) {
  Timestamp ts = Timestamp::hours(1);
  EXPECT_EQ(ts.seconds(), 3600);
}

TEST(TimestampTest, Addition) {
  Timestamp ts = Timestamp::hours(1);
  ts += Timespan::minutes(30);
  EXPECT_EQ(ts.minutes(), 90);
}

TEST(TimestampTest, Subtraction) {
  Timestamp ts = Timestamp::hours(2);
  ts -= Timespan::hours(1);
  EXPECT_EQ(ts.hours(), 1);
}

TEST(TimestampTest, Comparison) {
  Timestamp a = Timestamp::hours(1);
  Timestamp b = Timestamp::hours(2);
  EXPECT_TRUE(a < b);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(a == Timestamp::hours(1));
  EXPECT_TRUE(a != b);
}

TEST(TimestampTest, UtcLocal) {
  // 确保 utc() 和 local() 都返回合理值
  int64_t utc = Timestamp::utc();
  int64_t local = Timestamp::local();
  EXPECT_GT(utc, 0);
  EXPECT_GT(local, 0);
}

TEST(TimestampTest, Nano) {
  int64_t ns = Timestamp::nano();
  EXPECT_GT(ns, 0);
}

TEST(TimestampTest, ChronoConversion) {
  Timestamp ts = Timestamp::hours(24);
  auto tp = ts.chrono();
  Timestamp ts2(tp);
  EXPECT_EQ(ts.total(), ts2.total());
}
