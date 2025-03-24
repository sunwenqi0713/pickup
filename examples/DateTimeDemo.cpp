#include <iostream>

#include "pickup/time/DateTime.h"
#include "pickup/time/TimeSpan.h"

using namespace pickup::time;

void timespan_example() {
  TimeSpan ts1(3, 12, 30, 45);          // 3天12小时30分45秒
  std::chrono::seconds duration{3600};  // 1小时
  TimeSpan ts2(duration);

  TimeSpan sum = ts1 + ts2;
  std::cout << "Total hours: " << sum.totalHours() << std::endl;
  std::cout << "Formatted: " << sum.format("%D days %H:%M:%S") << std::endl;

  if (ts1 > ts2) {
    std::cout << "ts1 is longer than ts2" << std::endl;
  }
}

void datetime_example() {
  DateTime now = DateTime::now();
  std::cout << "Current time: " << now.format("%Y-%m-%d %H:%M:%S") << std::endl;

  DateTime dt(2023, 12, 25, 15, 30);
  std::cout << "Christmas: " << dt.format("%Y/%m/%d %H:%M") << std::endl;

  TimeSpan diff = now - dt;
  std::cout << "Difference: " << diff.hours() << " hours" << std::endl;
}

void combie_example() {
  DateTime dt1(2023, 12, 25);  // 圣诞节
  DateTime dt2 = DateTime::now();

  TimeSpan ts = dt2 - dt1;
  std::cout << "Days since Christmas: " << ts.days() << std::endl;

  DateTime future = dt2 + TimeSpan(30, 0, 0, 0);  // 加30天
  std::cout << "30 days later: " << future.format("%Y-%m-%d") << std::endl;

  TimeSpan ts2(3, 12, 30, 45);  // 3天12小时30分45秒
  dt2 += ts2;
  std::cout << "Updated time: " << dt2.format("%H:%M:%S") << std::endl;
}

int main() {
  timespan_example();
  datetime_example();
  combie_example();
  return 0;
}