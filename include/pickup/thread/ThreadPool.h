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

/**
 * @brief 线程池，用于并发执行任务
 *
 * 支持设置最大队列大小以限制任务堆积。
 */
class ThreadPool {
 public:
  using Task = std::function<void()>;

  explicit ThreadPool(const std::string& name = "");
  ~ThreadPool();

  /**
   * @brief 设置任务队列最大容量
   * @param maxSize 最大容量（0 表示无限制）
   * @note 必须在 start() 之前调用
   */
  void setMaxQueueSize(int maxSize) { maxQueueSize_ = maxSize; }

  /**
   * @brief 启动线程池
   * @param numThreads 工作线程数量
   */
  void start(size_t numThreads);

  /**
   * @brief 停止线程池，等待所有线程结束
   */
  void stop();

  /// 获取线程池名称
  const std::string& name() const { return name_; }

  /// 获取当前任务队列大小
  size_t queueSize() const;

  /**
   * @brief 添加任务到队列
   * @param task 要执行的任务
   * @note 如果队列已满，会阻塞等待；如果线程池已停止，立即返回
   */
  void addTask(Task task);

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
