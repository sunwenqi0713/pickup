#include <iostream>

#include "pickup/time/Time.h"
#include "pickup/time/Timespan.h"

using namespace pickup::time;

void time_example() {
  // 当前本地时间与 UTC 时间
  LocalTime now;
  UtcTime utcNow;
  std::cout << "Local time: " << now.toISO8601(true) << std::endl;
  std::cout << "UTC time:   " << utcNow.toISO8601(true) << std::endl;

  // 从字段构造（月 1–12）
  UtcTime ts(2021, 10, 1);  // 2021-10-01 00:00:00 UTC
  std::cout << "UtcTime from fields: " << ts.toISO8601(true) << std::endl;

  // 时间运算
  UtcTime ts1 = now + Timespan::seconds(1);
  std::cout << "Now + 1s: " << ts1.toISO8601(true) << std::endl;
  UtcTime ts2 = now - Timespan::seconds(1);
  std::cout << "Now - 1s: " << ts2.toISO8601(true) << std::endl;
}

void timespan_example() {
  // 创建时间跨度
  Timespan span1 = Timespan::hours(1) + Timespan::minutes(2) + Timespan::seconds(3);
  std::cout << "Timespan: " << span1.milliseconds() << " ms" << std::endl;

  Timespan span2 = Timespan::seconds(1);
  std::cout << "Timespan 1s: " << span2.milliseconds() << " ms" << std::endl;

  Timespan span3 = span1 + span2;
  std::cout << "Sum: " << span3.milliseconds() << " ms" << std::endl;
  Timespan span4 = span1 - span2;
  std::cout << "Diff: " << span4.milliseconds() << " ms" << std::endl;
}

void datetime_example() {
  // 从字段构造（月 1–12）
  UtcTime dt(2025, 3, 14, 10, 1, 17);
  std::cout << "DateTime: " << dt.year() << "-" << dt.month() << "-" << dt.day()
            << " " << dt.hour() << ":" << dt.minute() << ":" << dt.second() << std::endl;

  // 获取当前本地时间并输出各字段
  LocalTime now;
  std::cout << "Now: " << now.year() << "-" << now.month() << "-" << now.day()
            << " " << now.hour() << ":" << now.minute() << ":" << now.second() << std::endl;
}

int main() {
  time_example();
  timespan_example();
  datetime_example();
  return 0;
}
