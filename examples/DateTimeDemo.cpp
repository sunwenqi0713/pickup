#include <iostream>

#include "pickup/time/DateTime.h"
#include "pickup/time/TimeSpan.h"

using namespace pickup::time;

void timespan_example() {
  TimeSpan ts1(3, 12, 30, 45);          // 3天12小时30分45秒
  std::chrono::seconds duration{3600};  // 1小时
  TimeSpan ts2(duration);

  TimeSpan sum = ts1 + ts2;
  std::cout << "Total time: " << sum.total() << std::endl;

  if (ts1 > ts2) {
    std::cout << "ts1 is longer than ts2" << std::endl;
  }
}

void datetime_example() {
  DateTime now = DateTime::currentTime();
  std::cout << "Current year: " << now.year() << std::endl;
  std::cout << "Current month: " << now.month() << std::endl;
  std::cout << "Current day: " << now.day() << std::endl;
  std::cout << "Current hour: " << now.hours() << std::endl;
  std::cout << "Current minute: " << now.minutes() << std::endl;
  std::cout << "Current second: " << now.seconds() << std::endl;
  std::cout << "Current time: " << now.format("%Y-%m-%d %H:%M:%S.%f") << std::endl;

  DateTime dt(2023, 12, 25, 15, 30);
  std::cout << "Christmas: " << dt.format("%Y/%m/%d %H:%M") << std::endl;
  std::cout << "Is leap year: " << (dt.isLeapYear() ? "Yes" : "No") << std::endl;
  std::cout << "Day of week: " << dt.dayOfWeek() << std::endl;
  std::cout << "Day of year: " << dt.dayOfYear() << std::endl;
  std::cout << "Milliseconds since epoch: " << dt.currentTimeMillis() << std::endl;
}

int main() {
  timespan_example();
  datetime_example();
  return 0;
}