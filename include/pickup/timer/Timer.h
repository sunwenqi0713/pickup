#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <stdexcept>
#include <thread>

#include "pickup/timer/TimerTask.h"

namespace pickup {
namespace timer {

/**
 * @brief 将 std::function 适配为 TimerTask 的适配器
 */
class TimerTaskAdapter final : public TimerTask {
 public:
  explicit TimerTaskAdapter(std::function<void()> func) : func_(std::move(func)) {}
  void run() final { func_(); }

 private:
  std::function<void()> func_;
};

/**
 * @brief 定时器，支持一次性 / 重复执行的任务调度
 *
 * 内部维护一个专用线程，顺序执行到期的任务。
 */
class Timer {
 public:
  Timer();
  ~Timer();

  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  /**
   * @brief 停止定时器并等待工作线程退出
   * @note 不可在定时任务内部调用，否则抛出 std::runtime_error
   */
  void stop();

  /**
   * @brief 在指定延迟后执行一个函数
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param func  要执行的函数
   * @param delay 延迟时间
   */
  template <class Rep, class Period>
  void schedule(std::function<void()> func, const std::chrono::duration<Rep, Period>& delay) {
    schedule(std::make_shared<TimerTaskAdapter>(std::move(func)), delay);
  }

  /**
   * @brief 在指定延迟后执行一个定时任务
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param task  定时任务
   * @param delay 延迟时间
   */
  template <class Rep, class Period>
  void schedule(const TimerTaskPtr& task, const std::chrono::duration<Rep, Period>& delay) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (destroyed_) {
      throw std::invalid_argument("timer destroyed");
    }

    if (delay < std::chrono::nanoseconds::zero()) {
      throw std::invalid_argument("invalid negative delay");
    }

    auto now = std::chrono::steady_clock::now();
    auto time = now + delay;
    if (delay > std::chrono::nanoseconds::zero() && time < now) {
      throw std::invalid_argument("delay too large, resulting in overflow");
    }

    bool inserted = tasks_.insert(std::make_pair(task, time)).second;
    if (!inserted) {
      throw std::invalid_argument("task is already scheduled");
    }
    tokens_.insert({time, std::nullopt, task});

    if (wakeUpTime_ == std::chrono::steady_clock::time_point() || time < wakeUpTime_) {
      condition_.notify_one();
    }
  }

  /**
   * @brief 重新调度一个任务
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param task  要重新调度的任务
   * @param delay 新的延迟时间
   * @note 即使 task 之前未调度，此函数也会将其加入调度
   */
  template <class Rep, class Period>
  void reschedule(const TimerTaskPtr& task, const std::chrono::duration<Rep, Period>& delay) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (destroyed_) {
      throw std::invalid_argument("timer destroyed");
    }

    if (delay < std::chrono::nanoseconds::zero()) {
      throw std::invalid_argument("invalid negative delay");
    }

    auto now = std::chrono::steady_clock::now();
    auto time = now + delay;
    if (delay > std::chrono::nanoseconds::zero() && time < now) {
      throw std::invalid_argument("delay too large, resulting in overflow");
    }

    cancelNoSync(task);

    tasks_.insert(std::make_pair(task, time));
    tokens_.insert({time, std::nullopt, task});

    if (wakeUpTime_ == std::chrono::steady_clock::time_point() || time < wakeUpTime_) {
      condition_.notify_one();
    }
  }

  /**
   * @brief 调度一个重复执行的任务
   * @tparam Rep 延迟时间类型
   * @tparam Period 延迟时间精度
   * @param task  要重复执行的任务
   * @param delay 每次执行之间的间隔
   */
  template <class Rep, class Period>
  void scheduleRepeated(const TimerTaskPtr& task, const std::chrono::duration<Rep, Period>& delay) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (destroyed_) {
      throw std::invalid_argument("timer destroyed");
    }

    if (delay < std::chrono::nanoseconds::zero()) {
      throw std::invalid_argument("invalid negative delay");
    }

    auto now = std::chrono::steady_clock::now();
    auto time = now + delay;
    if (delay > std::chrono::nanoseconds::zero() && time < now) {
      throw std::invalid_argument("delay too large, resulting in overflow");
    }

    bool inserted = tasks_.insert(std::make_pair(task, time)).second;
    if (!inserted) {
      throw std::invalid_argument("task is already scheduled");
    }
    tokens_.insert({time, std::chrono::duration_cast<std::chrono::nanoseconds>(delay), task});

    if (wakeUpTime_ == std::chrono::steady_clock::time_point() || time < wakeUpTime_) {
      condition_.notify_one();
    }
  }

  /**
   * @brief 取消一个任务
   * @param task 要取消的任务
   * @return 成功取消返回 true；任务尚未执行、已取消或从未调度返回 false
   */
  bool cancel(const TimerTaskPtr& task) noexcept;

  /** @brief 检查任务是否已调度 */
  bool isScheduled(const TimerTaskPtr& task);

 protected:
  /**
   * @brief 执行定时任务
   * @param task 要执行的任务
   * @note 子类可重写以自定义执行逻辑
   */
  virtual void executeTask(const TimerTaskPtr& task);

 private:
  struct Token {
    std::chrono::steady_clock::time_point scheduledTime;
    std::optional<std::chrono::nanoseconds> delay;
    TimerTaskPtr task;

    bool operator<(const Token& other) const {
      if (scheduledTime < other.scheduledTime) return true;
      if (scheduledTime > other.scheduledTime) return false;
      return task < other.task;
    }
  };

  void runLoop();
  bool cancelNoSync(const TimerTaskPtr& task) noexcept;

  std::mutex mutex_;
  std::condition_variable condition_;
  std::set<Token> tokens_;
  std::map<TimerTaskPtr, std::chrono::steady_clock::time_point> tasks_;
  bool destroyed_{false};
  std::chrono::steady_clock::time_point wakeUpTime_;
  std::thread worker_;
};

}  // namespace timer
}  // namespace pickup
