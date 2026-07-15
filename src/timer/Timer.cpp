#include "pickup/timer/Timer.h"

#include <cassert>
#include <exception>
#include <stdexcept>
#include <thread>

namespace pickup {
namespace timer {

TimerTask::~TimerTask() = default;

Timer::Timer() : wakeUpTime_(std::chrono::steady_clock::time_point{}), worker_(&Timer::runLoop, this) {}

Timer::~Timer() {
  if (worker_.joinable()) {
    try {
      stop();
    } catch (...) {
      // 析构函数不能抛异常
    }
  }
}

void Timer::stop() {
  if (std::this_thread::get_id() == worker_.get_id()) {
    throw std::runtime_error("a timer task cannot destroy the timer");
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (destroyed_) {
      return;
    }
    destroyed_ = true;
    tasks_.clear();
    tokens_.clear();
    condition_.notify_one();
  }
  worker_.join();
}

bool Timer::cancel(const TimerTaskPtr& task) noexcept {
  std::lock_guard<std::mutex> lock(mutex_);
  return cancelNoSync(task);
}

bool Timer::isScheduled(const TimerTaskPtr& task) const {
  std::lock_guard<std::mutex> lock(mutex_);
  if (destroyed_) {
    return false;
  }
  return tasks_.find(task) != tasks_.end();
}

void Timer::addTask(const TimerTaskPtr& task, std::chrono::nanoseconds delay,
                    std::optional<std::chrono::nanoseconds> repeat, bool reschedule,
                    bool fixedRate) {
  // 前置条件（调用方 bug）：延迟须非负且不致时间点溢出。以断言而非异常处理——
  // 调试期即时暴露，release 期零开销。
  assert(delay >= std::chrono::nanoseconds::zero() && "Timer: delay must be non-negative");

  std::lock_guard<std::mutex> lock(mutex_);
  if (destroyed_) {
    throw std::invalid_argument("timer destroyed");
  }

  const auto now = std::chrono::steady_clock::now();
  const auto time = now + delay;
  assert(!(delay > std::chrono::nanoseconds::zero() && time < now) &&
         "Timer: delay too large, causes time_point overflow");

  // reschedule 允许重排已有任务，因此先移除旧条目再插入（插入必然成功）；
  // 其余情形下重复调度同一任务视为错误。
  if (reschedule) {
    cancelNoSync(task);
  }
  auto [it, inserted] = tasks_.insert({task, time});
  if (!inserted) {
    throw std::invalid_argument("task is already scheduled");
  }
  // 若 token 插入失败（如 bad_alloc），回滚 tasks_ 条目，保证两索引一致
  try {
    tokens_.insert({time, repeat, task, fixedRate});
  } catch (...) {
    tasks_.erase(it);
    throw;
  }

  // 仅当新任务比当前等待目标更早（或工作线程正空等）时才唤醒，避免无谓唤醒
  if (wakeUpTime_ == std::chrono::steady_clock::time_point{} || time < wakeUpTime_) {
    condition_.notify_one();
  }
}

void Timer::executeTask(const TimerTaskPtr& task) { task->run(); }

// 默认吞掉任务异常（库不向任何流输出）；子类可重写以记录日志等。
void Timer::onException(const TimerTaskPtr& /*task*/, std::exception_ptr /*error*/) noexcept {}

void Timer::runLoop() {
  Token token{std::chrono::steady_clock::time_point{}, std::nullopt, nullptr};
  while (true) {
    {
      std::unique_lock<std::mutex> lock(mutex_);

      if (!destroyed_) {
        // 如果刚执行完的是重复任务，重新加入调度。执行期间锁已释放，其间可能有并发
        // 的 cancel/reschedule 改动该任务，故仅当 map 中的排期仍是本次执行所依据的
        // 那一次（p->second 未变）时才自动续排；否则以并发改动为准，避免残留重复
        // token、破坏重排语义。
        if (token.delay) {
          auto p = tasks_.find(token.task);
          if (p != tasks_.end() && p->second == token.scheduledTime) {
            // fixed-rate 以上次计划时刻为基准（不受执行耗时影响，落后时靠后续补触发
            // 追赶）；fixed-delay 以完成时刻为基准。
            token.scheduledTime = token.fixedRate
                                      ? token.scheduledTime + token.delay.value()
                                      : std::chrono::steady_clock::now() + token.delay.value();
            p->second = token.scheduledTime;
            tokens_.insert(token);
          }
        }
        token = {std::chrono::steady_clock::time_point{}, std::nullopt, nullptr};

        if (tokens_.empty()) {
          wakeUpTime_ = std::chrono::steady_clock::time_point{};
          condition_.wait(lock, [this] { return destroyed_ || !tokens_.empty(); });
        }
      }

      if (destroyed_) {
        break;
      }

      while (!tokens_.empty() && !destroyed_) {
        const auto now = std::chrono::steady_clock::now();
        const Token& first = *(tokens_.begin());
        if (first.scheduledTime <= now) {
          token = first;
          tokens_.erase(tokens_.begin());
          if (!token.delay) {
            tasks_.erase(token.task);
          }
          break;
        }

        wakeUpTime_ = first.scheduledTime;
        condition_.wait_until(lock, first.scheduledTime);
      }

      if (destroyed_) {
        break;
      }
    }

    if (token.task) {
      // 异常在锁外交给可覆盖的钩子处理，避免单个任务失败中断定时线程
      try {
        executeTask(token.task);
      } catch (...) {
        onException(token.task, std::current_exception());
      }

      if (!token.delay) {
        // 对于一次性任务，清除 task 引用以避免死锁（task 的析构可能触发用户代码）
        token.task = nullptr;
      }
    }
  }
}

bool Timer::cancelNoSync(const TimerTaskPtr& task) noexcept {
  if (destroyed_) {
    return false;
  }

  auto p = tasks_.find(task);
  if (p == tasks_.end()) {
    return false;
  }

  tokens_.erase(Token{p->second, std::nullopt, p->first});
  tasks_.erase(p);

  return true;
}

}  // namespace timer
}  // namespace pickup