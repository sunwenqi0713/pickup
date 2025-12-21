#pragma once

#pragma once

#include <format>

#include <array>
#include <chrono>
#include <string>

#include "pickup/time/Duration.hpp"

namespace pickup {
namespace utils {
// 用于测量时间间隔的辅助类。默认使用 StopWatch 别名表示单调时钟。
//
// 基本用法：
//
//   StopWatch sw(StopWatch::STARTED);
//   doSomething();
//   LOG("操作耗时 {}ms", sw.elapsedMs());
//
// 如果需要更手动地控制秒表，可以显式调用 start() 和 stop()。
// 默认情况下，秒表在停止状态下启动，需要调用 start() 来开始计时。
//
//   StopWatch sw;
//   sw.start();
//   doSomething();
//   sw.stop();
//   LOG("操作耗时 {}ms", sw.elapsedMs());
//
// 此类对于运行状态的更改不是线程安全的。如果打算在线程之间共享此对象
// （例如异步回调中更新 StopWatch 的运行状态），必须确保只有一个线程
// 在任何时候更新该类，包括读取器。对 StopWatch 的并发读取总是线程安全的。
template <class Clock>
class StopWatchGeneric {
 public:
  enum Mode {
    STOPPED = 0,
    STARTED = 1,
  };

  // 消息测试代码依赖下面的隐式构造函数以获得更短的代码。
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr StopWatchGeneric(const Mode startMode = StopWatchGeneric::STOPPED) {
    if (startMode == STARTED) {
      startingTime_ = now();
      running_ = true;
    }
  }

  // 开始或恢复测量时间。如果之前已经启动/停止过，
  // 新测量的经过时间将与之前累计的经过时间相加。
  // 如果已经在运行中，尝试再次启动将不做任何操作。
  void start() {
    if (running_) {
      return;
    }

    startingTime_ = now();
    running_ = true;
  }

  // 停止测量经过时间。不会重置已累计的经过时间。
  void stop() {
    if (!running_) {
      return;
    }
    elapsedTotal_ += fromStart();
    running_ = false;
  }

  // 停止时间间隔测量并将经过时间重置为零。
  void reset() {
    elapsedTotal_ = {};
    running_ = false;
  }

  // 辅助方法，先执行 reset() 然后执行 start()
  void restart() {
    elapsedTotal_ = {};
    startingTime_ = now();
    running_ = true;
  }

  // 获取经过的秒数（整数部分）
  int64_t elapsedSeconds() const { return static_cast<int64_t>(elapsed().seconds()); }

  // 获取经过的毫秒数
  int64_t elapsedMs() const { return static_cast<int64_t>(elapsed().milliseconds()); }

  // 获取经过的微秒数
  int64_t elapsedUs() const { return static_cast<int64_t>(elapsed().microseconds()); }

  // 获取秒表运行时的总经过时间
  Duration<Clock> elapsed() const {
    auto elapsed = elapsedTotal_;

    if (running_) elapsed += fromStart();

    return Duration<Clock>(elapsed);
  }

  const typename Clock::time_point& getStartTime() const { return startingTime_; }

 private:
  typename Clock::duration fromStart() const { return now() - startingTime_; }

  static typename Clock::time_point now() noexcept { return Clock::now(); }

  typename Clock::duration elapsedTotal_{};
  typename Clock::time_point startingTime_{};
  bool running_ = false;
};

// 用于单调时钟的秒表。保证经过的时间永远不会为负值。
using StopWatch = StopWatchGeneric<std::chrono::steady_clock>;

}  // namespace utils
}  // namespace pickup