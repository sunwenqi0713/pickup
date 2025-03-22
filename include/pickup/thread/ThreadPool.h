#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace pickup {
namespace thread {

class ThreadPool {
 public:
  using Task = std::function<void()>;

  explicit ThreadPool(const std::string& name = "");
  ~ThreadPool();

  // Must be called before start().
  void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }

  void start(size_t numThreads);
  void stop();

  const std::string& name() const { return name_; }

  size_t queueSize() const;

  // Could block if maxQueueSize > 0
  // Call after stop() will return immediately.
  // There is no move-only version of std::function in C++ as of C++14.
  // So we don't need to overload a const& and an && versions
  // as we do in (Bounded)BlockingQueue.
  // https://stackoverflow.com/a/25408989
  void addTask(Task f);

 private:
  bool isFull() const;
  void threadFunc();
  Task take();

 private:
  std::string name_;
  mutable std::mutex mutex_;
  std::condition_variable notEmpty_;
  std::condition_variable notFull_;
  std::vector<std::thread> threads_;
  std::deque<Task> queue_;
  size_t maxQueueSize_;
  bool running_;
};

}  // namespace thread
}  // namespace pickup
