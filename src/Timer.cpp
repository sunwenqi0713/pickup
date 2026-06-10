#include "pickup/timer/Timer.h"

#include <iostream>
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

bool Timer::isScheduled(const TimerTaskPtr& task) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (destroyed_) {
    return false;
  }
  return tasks_.find(task) != tasks_.end();
}

void Timer::executeTask(const TimerTaskPtr& task) { task->run(); }

void Timer::runLoop() {
  Token token{std::chrono::steady_clock::time_point{}, std::nullopt, nullptr};
  while (true) {
    {
      std::unique_lock<std::mutex> lock(mutex_);

      if (!destroyed_) {
        // 如果刚执行完的是重复任务，重新加入调度（若未被取消）
        if (token.delay) {
          auto p = tasks_.find(token.task);
          if (p != tasks_.end()) {
            token.scheduledTime = std::chrono::steady_clock::now() + token.delay.value();
            p->second = token.scheduledTime;
            tokens_.insert(token);
          }
        }
        token = {std::chrono::steady_clock::time_point{}, std::nullopt, nullptr};

        if (tokens_.empty()) {
          wakeUpTime_ = std::chrono::steady_clock::time_point{};
          condition_.wait(lock);
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
        condition_.wait_for(lock, first.scheduledTime - now);
      }

      if (destroyed_) {
        break;
      }
    }

    if (token.task) {
      try {
        executeTask(token.task);
      } catch (const std::exception& e) {
        std::cerr << "Timer::runLoop(): uncaught exception: " << e.what() << std::endl;
      } catch (...) {
        std::cerr << "Timer::runLoop(): uncaught unknown exception" << std::endl;
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
