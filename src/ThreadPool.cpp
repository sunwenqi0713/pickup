#include "pickup/thread/ThreadPool.h"

#include <cassert>

#include "pickup/thread/Thread.h"

namespace pickup {
namespace thread {

namespace {
constexpr size_t kMaxThreadNum = 16;
constexpr size_t kDefaultThreadNum = 1;
}  // namespace

ThreadPool::ThreadPool(const std::string& name) : name_(name), maxQueueSize_(0), running_(false) {}

ThreadPool::~ThreadPool() {
  if (running_) {
    stop();
  }
}

void ThreadPool::start(size_t numThreads) {
  if (!threads_.empty()) {
    return;  // 已启动，忽略
  }

  if (numThreads == 0) {
    numThreads = kDefaultThreadNum;
  }

  if (numThreads > kMaxThreadNum) {
    numThreads = kMaxThreadNum;
  }

  running_ = true;
  threads_.reserve(numThreads);
  for (int i = 0; i < numThreads; ++i) {
    threads_.emplace_back([this, id = i]() {
      // Give the name of ThreadPool to threads created by the ThreadPool.
      setThreadName((name_ + std::to_string(id)).c_str());
      threadFunc();
    });
  }
}

void ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    running_ = false;
    notEmpty_.notify_all();
    notFull_.notify_all();
  }
  for (auto& t : threads_) {
    t.join();
  }
}

size_t ThreadPool::queueSize() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return queue_.size();
}

void ThreadPool::addTask(Task task) {
  if (threads_.empty()) {
    task();
  } else {
    std::unique_lock<std::mutex> lock(mutex_);
    while (isFull() && running_) {
      notFull_.wait(lock);
    }
    if (!running_) return;
    assert(!isFull());

    queue_.push_back(std::move(task));
    notEmpty_.notify_one();
  }
}

ThreadPool::Task ThreadPool::take() {
  std::unique_lock<std::mutex> lock(mutex_);
  // always use a while-loop, due to spurious wakeup
  while (queue_.empty() && running_) {
    notEmpty_.wait(lock);
  }
  Task task;
  if (!queue_.empty()) {
    task = queue_.front();
    queue_.pop_front();
    if (maxQueueSize_ > 0) {
      notFull_.notify_one();
    }
  }
  return task;
}

bool ThreadPool::isFull() const { return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_; }

void ThreadPool::threadFunc() {
  while (running_) {
    Task task(take());
    if (task) {
      task();
    }
  }
}

}  // namespace thread
}  // namespace pickup