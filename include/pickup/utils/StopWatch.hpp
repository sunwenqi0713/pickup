#pragma once

#include <array>
#include <chrono>
#include <format>
#include <string>

namespace pickup {
namespace utils {
/**
 * @brief 时间间隔测量工具类（秒表）
 *
 * @details
 * StopWatchGeneric 用于测量代码执行过程中消耗的时间间隔。
 * 该类支持多次 start / stop，所有运行期间的时间会被累加。
 *
 * 通常不直接使用该模板类，而是使用其别名 StopWatch，
 * 后者基于单调时钟（std::chrono::steady_clock），
 * 能保证测量结果不会出现负值。
 *
 * @tparam Clock 使用的时钟类型（需满足 std::chrono Clock 要求）
 *
 * @note
 * - 本类在"运行状态（start/stop/reset）"的修改上 **不是线程安全的**
 * - 并发读取 elapsed() 是线程安全的
 * - 若跨线程更新运行状态，需由调用方自行保证同步
 */
template <class Clock>
class StopWatchGeneric {
 public:
  using Duration = typename Clock::duration;
  using TimePoint = typename Clock::time_point;

  /**
   * @brief 秒表初始状态
   */
  enum Mode {
    Stopped = 0, /**< 初始为停止状态 */
    Started = 1, /**< 构造时立即开始计时 */
  };

  /**
   * @brief 构造函数
   *
   * @param startMode 指定构造时是否立即启动秒表
   *
   * @details
   * - 默认构造为 Stopped 状态
   * - 若传入 Started，则在构造时记录当前时间并开始计时
   *
   * @note
   * 该构造函数被设计为可隐式调用，用于简化测试代码书写
   */
  // NOLINTNEXTLINE(google-explicit-constructor)
  constexpr StopWatchGeneric(const Mode startMode = StopWatchGeneric::Stopped) {
    if (startMode == Started) {
      startingTime_ = now();
      running_ = true;
    }
  }

  /**
   * @brief 开始或继续计时
   *
   * @details
   * - 如果秒表当前为停止状态，则开始计时
   * - 如果之前已经有累计时间，新计时会在此基础上累加
   * - 若当前已处于运行状态，则该调用不会产生任何效果
   */
  void start() {
    if (running_) {
      return;
    }

    startingTime_ = now();
    running_ = true;
  }

  /**
   * @brief 停止计时
   *
   * @details
   * - 将当前运行区间的时间累加到总耗时
   * - 不会清空已累计的时间
   * - 若当前未运行，则该调用不会产生任何效果
   */
  void stop() {
    if (!running_) {
      return;
    }
    elapsedTotal_ += fromStart();
    running_ = false;
  }

  /**
   * @brief 重置秒表
   *
   * @details
   * - 清空所有已累计的耗时
   * - 将秒表置为停止状态
   */
  void reset() {
    elapsedTotal_ = {};
    running_ = false;
  }

  /**
   * @brief 重新开始计时
   *
   * @details
   * 等价于先调用 reset()，再调用 start()
   */
  void restart() {
    elapsedTotal_ = {};
    startingTime_ = now();
    running_ = true;
  }

  /**
   * @brief 获取已消耗的总时间（秒）
   *
   * @return 已累计的秒数
   */
  int64_t elapsedSeconds() const { return std::chrono::duration_cast<std::chrono::seconds>(elapsed()).count(); }

  /**
   * @brief 获取已消耗的总时间（毫秒）
   *
   * @return 已累计的毫秒数
   */
  int64_t elapsedMs() const { return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed()).count(); }

  /**
   * @brief 获取已消耗的总时间（微秒）
   *
   * @return 已累计的微秒数
   */
  int64_t elapsedUs() const { return std::chrono::duration_cast<std::chrono::microseconds>(elapsed()).count(); }

  /**
   * @brief 获取已消耗的总时间
   *
   * @details
   * - 返回秒表自启动以来的累计耗时
   * - 若当前仍在运行，会将"当前运行区间"的时间一并计入
   *
   * @return Duration 类型的时间间隔
   */
  Duration elapsed() const {
    auto elapsed = elapsedTotal_;

    if (running_) elapsed += fromStart();

    return elapsed;
  }

  /**
   * @brief 获取最近一次 start() 时的起始时间点
   *
   * @return 时钟的 time_point
   */
  const TimePoint& startTime() const { return startingTime_; }

  /**
   * @brief 检查秒表是否正在运行
   *
   * @return 如果正在运行返回 true，否则返回 false
   */
  bool isRunning() const { return running_; }

 private:
  /**
   * @brief 计算从最近一次 start() 到当前的时间间隔
   */
  Duration fromStart() const { return now() - startingTime_; }

  /**
   * @brief 获取当前时间点
   */
  static TimePoint now() noexcept { return Clock::now(); }

  Duration elapsedTotal_{};  /**< 已累计的总耗时 */
  TimePoint startingTime_{}; /**< 最近一次启动的时间点 */
  bool running_ = false;     /**< 当前是否处于运行状态 */
};

/**
 * @brief 基于单调时钟的秒表类型
 *
 * @details
 * 使用 std::chrono::steady_clock，保证时间单调递增，
 * 不受系统时间调整影响，适合用于性能统计与耗时测量。
 */
using StopWatch = StopWatchGeneric<std::chrono::steady_clock>;

}  // namespace utils
}  // namespace pickup