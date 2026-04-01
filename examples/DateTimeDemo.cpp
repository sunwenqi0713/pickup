#include <iostream>

#include "pickup/time/Time.h"
#include "pickup/time/Timespan.h"

using namespace pickup::time;

void time_example() {
  // 获取当前时间
  Time now = Time::getCurrentTime();
  std::cout << "Current time ms since epoch: " << now.toMilliseconds() << std::endl;

  // 从毫秒数构造
  Time ts(1633036800000LL);  // 2021-10-01 00:00:00 UTC
  std::cout << "Time from ms: " << ts.toMilliseconds() << " ms since epoch" << std::endl;

  // 时间运算
  Time ts1 = now + Timespan::seconds(1.0);
  std::cout << "After +1s: " << ts1.toMilliseconds() << " ms since epoch" << std::endl;
  Time ts2 = now - Timespan::seconds(1.0);
  std::cout << "After -1s: " << ts2.toMilliseconds() << " ms since epoch" << std::endl;
}

void timespan_example() {
  // 创建时间跨度
  Timespan span1 = Timespan::hours(1.0) + Timespan::minutes(2.0) + Timespan::seconds(3.0);
  std::cout << "Timespan: " << span1.inMilliseconds() << " ms" << std::endl;

  Timespan span2 = Timespan::seconds(1.0);
  std::cout << "Timespan 1s: " << span2.inMilliseconds() << " ms" << std::endl;

  Timespan span3 = span1 + span2;
  std::cout << "Sum: " << span3.inMilliseconds() << " ms" << std::endl;
  Timespan span4 = span1 - span2;
  std::cout << "Diff: " << span4.inMilliseconds() << " ms" << std::endl;
}

void datetime_example() {
  // 从字段构造（月份 0-11）
  Time dt(2025, 3, 14, 10, 1, 17);
  auto bd = dt.toBrokenDown();
  std::cout << "DateTime: " << bd.year << "-" << (bd.month + 1) << "-" << bd.day
            << " " << bd.hours << ":" << bd.minutes << ":" << bd.seconds << std::endl;
  std::cout << "Ms since epoch: " << dt.toMilliseconds() << std::endl;

  // 获取当前时间并分解字段
  Time now = Time::getCurrentTime();
  auto now_bd = now.toBrokenDown();
  std::cout << "Now: " << now_bd.year << "-" << (now_bd.month + 1) << "-" << now_bd.day
            << " " << now_bd.hours << ":" << now_bd.minutes << ":" << now_bd.seconds << std::endl;
}

int main() {
  time_example();
  timespan_example();
  datetime_example();
  return 0;
}
