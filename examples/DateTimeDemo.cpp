#include <iostream>

#include "pickup/time/DateTime.h"
#include "pickup/time/Timespan.h"
#include "pickup/time/Timestamp.h"

using namespace pickup::time;

void timestamp_example() {
  // 创建当前时间戳
  Timestamp now;
  std::cout << "Current Timestamp: " << now.epochMilliseconds() << " milliseconds since epoch" << std::endl;
  // 创建指定时间戳
  Timestamp ts(1633036800000);  // 2021-10-01 00:00:00 UTC
  std::cout << "Timestamp from milliseconds: " << ts.epochMilliseconds() << " milliseconds since epoch" << std::endl;
  // 时间戳运算
  Timestamp ts1 = now + 1000000;  // 加1秒
  std::cout << "Timestamp after adding 1 second: " << ts1.epochMilliseconds() << " milliseconds since epoch"
            << std::endl;
  Timestamp ts2 = now - 1000000;  // 减1秒
  std::cout << "Timestamp after subtracting 1 second: " << ts2.epochMilliseconds() << " milliseconds since epoch"
            << std::endl;
}

void timespan_example() {
  // 创建时间跨度
  Timespan span1(1, 2, 3, 4, 5);  // 1天2小时3分钟4秒5微秒
  std::cout << "Timespan: " << span1.totalMilliseconds() << " milliseconds" << std::endl;
  // 创建时间跨度（微秒）
  Timespan span2(1000000);  // 1秒
  std::cout << "Timespan from microseconds: " << span2.totalMilliseconds() << " milliseconds" << std::endl;
  // 创建时间跨度（秒和微秒）
  Timespan span3(1, 500000);  // 1秒500毫秒
  std::cout << "Timespan from seconds and microseconds: " << span3.totalMilliseconds() << " milliseconds" << std::endl;
  // 创建时间跨度（天、小时、分钟、秒、微秒）
  Timespan span4(1, 2, 3, 4, 5);  // 1天2小时3分钟4秒5微秒
  std::cout << "Timespan from days, hours, minutes, seconds, microseconds: " << span4.totalMilliseconds()
            << " milliseconds" << std::endl;
  // 时间跨度运算
  Timespan span5 = span1 + span2;  // 相加
  std::cout << "Sum of timespans: " << span5.totalMilliseconds() << " milliseconds" << std::endl;
  Timespan span6 = span1 - span2;  // 相减
  std::cout << "Difference of timespans: " << span6.totalMilliseconds() << " milliseconds" << std::endl;
}

void datetime_example() {
  // 创建 DateTime 对象
  DateTime dt(2025, 4, 14, 10, 1, 17);  // 2021-08-01 12:30:45
  std::cout << "DateTime: " << dt.year() << "-" << dt.month() << "-" << dt.day() << " " << dt.hour() << ":"
            << dt.minute() << ":" << dt.second() << std::endl;
  // 转换为 UTC 时间戳
  std::cout << "Timestamp from DateTime: " << dt.timestamp() << " microsecodes since epoch" << std::endl;
  // 获取当前时间
  DateTime now = DateTime::now();
  std::cout << "Current DateTime: " << now.year() << "-" << now.month() << "-" << now.day() << " " << now.hour() << ":"
            << now.minute() << ":" << now.second() << std::endl;
  std::cout << "Timestamp now from DateTime: " << now.timestamp() << " microsecodes since epoch" << std::endl;
  // 获取当前 UTC 时间
  DateTime utc_now = DateTime::nowUTC();
  std::cout << "Current UTC DateTime: " << utc_now.year() << "-" << utc_now.month() << "-" << utc_now.day() << " "
            << utc_now.hour() << ":" << utc_now.minute() << ":" << utc_now.second() << std::endl;
}

int main() {
  timestamp_example();
  timespan_example();
  datetime_example();
  return 0;
}