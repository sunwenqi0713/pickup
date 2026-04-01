#include "pickup/thread/ThreadPool.h"

#include <algorithm>
#include <cassert>

#include "pickup/thread/Thread.h"

namespace pickup {
namespace thread {

namespace {
// 默认线程数 = CPU 核心数（至少 1）
const size_t kDefaultThreadNum =
    std::max(static_cast<size_t>(std::thread::hardware_concurrency()), size_t{1});
// 最大线程数 = max(核心数 × 2, 16)，防止过度创建线程
const size_t kMaxThreadNum =
    std::max(kDefaultThreadNum * 2, size_t{16});
}  // namespace

ThreadPool::ThreadPool(const std::string& name) : name_(name) {}

ThreadPool::~ThreadPool() {
  stop();
}

void ThreadPool::start(size_t numThreads) {
  // compare_exchange_strong 保证并发调用只有一个能进入启动逻辑
  bool expected = false;
  if (!started_.compare_exchange_strong(expected, true)) {
    return;  // 已启动，幂等返回
  }

  if (numThreads == 0) numThreads = kDefaultThreadNum;
  if (numThreads > kMaxThreadNum) numThreads = kMaxThreadNum;

  running_.store(true);
  threads_.reserve(numThreads);
  for (size_t i = 0; i < numThreads; ++i) {
    threads_.emplace_back([this, id = i]() {
      setThreadName((name_ + std::to_string(id)).c_str());
      threadFunc();
    });
  }
}

void ThreadPool::stop() {
  // exchange 保证只有第一个调用者真正执行停止逻辑，后续调用幂等返回
  if (!running_.exchange(false)) {
    return;
  }

  // 唤醒 take()/addTask() 中等待线程，使其感知 running_ = false
  {
    std::lock_guard<std::mutex> lock(mutex_);
    notEmpty_.notify_all();
    notFull_.notify_all();
  }

  for (auto& t : threads_) {
    if (t.joinable()) t.join();
  }
  threads_.clear();

  // 清空残留队列并唤醒 waitForAllDone() 调用方
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.clear();
  }
  pendingCount_.store(0);
  drainCv_.notify_all();

  // 允许 stop() 后重新调用 start()
  started_.store(false);
}

size_t ThreadPool::queueSize() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return queue_.size();
}

void ThreadPool::addTask(Task task) {
  if (!running_.load()) return;

  std::unique_lock<std::mutex> lock(mutex_);
  // 队列满时阻塞等待，直到有空位或线程池停止
  notFull_.wait(lock, [this] { return !isFull() || !running_.load(); });
  if (!running_.load()) return;

  ++pendingCount_;
  queue_.push_back(std::move(task));
  notEmpty_.notify_one();
}

bool ThreadPool::tryAddTask(Task task) {
  if (!running_.load()) return false;

  std::lock_guard<std::mutex> lock(mutex_);
  if (isFull() || !running_.load()) return false;

  ++pendingCount_;
  queue_.push_back(std::move(task));
  notEmpty_.notify_one();
  return true;
}

bool ThreadPool::tryAddTask(Task task, std::chrono::milliseconds timeout) {
  if (!running_.load()) return false;

  std::unique_lock<std::mutex> lock(mutex_);
  if (!notFull_.wait_for(lock, timeout,
                         [this] { return !isFull() || !running_.load(); })) {
    return false;  // 超时
  }
  if (!running_.load()) return false;

  ++pendingCount_;
  queue_.push_back(std::move(task));
  notEmpty_.notify_one();
  return true;
}

void ThreadPool::waitForAllDone() {
  // 使用独立 drainMutex_，避免持有 mutex_ 阻塞 addTask
  std::unique_lock<std::mutex> lock(drainMutex_);
  drainCv_.wait(lock, [this] { return pendingCount_.load() == 0; });
}

std::optional<ThreadPool::Task> ThreadPool::take() {
  std::unique_lock<std::mutex> lock(mutex_);
  // 使用 while 循环防止虚假唤醒
  while (queue_.empty() && running_.load()) {
    notEmpty_.wait(lock);
  }
  if (queue_.empty()) {
    return std::nullopt;  // 线程池停止，通知 threadFunc 退出
  }
  Task task = std::move(queue_.front());
  queue_.pop_front();
  if (maxQueueSize_ > 0) {
    notFull_.notify_one();
  }
  return task;
}

void ThreadPool::threadFunc() {
  while (auto task = take()) {
    (*task)();
    // 任务执行完毕后减少计数；降至 0 时通知 waitForAllDone()
    if (--pendingCount_ == 0) {
      drainCv_.notify_all();
    }
  }
}

bool ThreadPool::isFull() const {
  return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

}  // namespace thread
}  // namespace pickup
