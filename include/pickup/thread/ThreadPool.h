#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

namespace pickup {
namespace thread {

/**
 * @brief 线程池
 *
 * 特性：
 * - start()/stop() 线程安全、幂等，支持 stop 后重新 start
 * - 支持有界/无界任务队列（setMaxQueueSize）
 * - 支持提交带返回值的任务（submit → std::future）
 * - 支持非阻塞/带超时的任务提交（tryAddTask）
 * - 支持等待当前队列全部执行完毕（waitForAllDone）
 * - stop() 为硬停止：已入队但未执行的任务会被丢弃
 *
 * 典型用法：
 * @code
 * ThreadPool pool("worker");
 * pool.start(4);
 *
 * auto future = pool.submit([] { return 42; });
 * pool.addTask([] { doSomething(); });
 *
 * pool.waitForAllDone();
 * pool.stop();
 * @endcode
 */
class ThreadPool {
 public:
  using Task = std::function<void()>;

  explicit ThreadPool(const std::string& name = "");
  ~ThreadPool();

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  /**
   * @brief 设置任务队列最大容量（必须在 start() 之前调用）
   * @param maxSize 最大容量（0 表示无限制）
   */
  void setMaxQueueSize(size_t maxSize) { maxQueueSize_ = maxSize; }

  /**
   * @brief 启动线程池（线程安全，幂等）
   * @param numThreads 工作线程数量（0 = hardware_concurrency）
   */
  void start(size_t numThreads = 0);

  /**
   * @brief 停止线程池，等待所有线程退出（线程安全，幂等）
   * @note 队列中尚未执行的任务会被丢弃。如需等待队列清空，先调用 waitForAllDone()
   */
  void stop();

  /** @brief 线程池名称 */
  const std::string& name() const { return name_; }

  /** @brief 当前任务队列大小 */
  size_t queueSize() const;

  /** @brief 是否正在运行 */
  bool isRunning() const { return running_.load(); }

  /**
   * @brief 提交任务（阻塞等待直到队列有空位）
   * @param task 要执行的任务
   * @note 线程池未启动或已停止时，任务被静默丢弃
   */
  void addTask(Task task);

  /**
   * @brief 非阻塞提交任务
   * @return 成功入队返回 true，队列已满或线程池未运行返回 false
   */
  bool tryAddTask(Task task);

  /**
   * @brief 带超时的提交任务
   * @param task    要执行的任务
   * @param timeout 最长等待时间
   * @return 成功入队返回 true，超时或线程池未运行返回 false
   */
  bool tryAddTask(Task task, std::chrono::milliseconds timeout);

  /**
   * @brief 提交带返回值的任务
   * @return std::future 可获取结果或捕获任务内部的异常
   * @note 线程池未运行时，返回的 future 调用 get() 会抛出异常
   */
  template <typename F, typename... Args>
  auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using R = std::invoke_result_t<F, Args...>;
    /** @brief packaged_task 包装异常，任务内部异常通过 future 传递给调用方 */
    auto ptask = std::make_shared<std::packaged_task<R()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<R> future = ptask->get_future();
    addTask([ptask]() { (*ptask)(); });
    return future;
  }

  /**
   * @brief 等待当前所有已提交的任务全部执行完毕
   * @note 等待期间仍可继续提交新任务；stop() 后会立即返回
   */
  void waitForAllDone();

 private:
  /**
   * @brief 从队列取出一个任务（阻塞直到有任务或停止）
   * @return 任务；线程池停止时返回 nullopt
   */
  std::optional<Task> take();

  void threadFunc();

  /** @brief 判断队列是否已满（调用方须持有 mutex_） */
  bool isFull() const;

 private:
  std::string name_;

  mutable std::mutex mutex_;        ///< 保护 queue_
  std::condition_variable notEmpty_;
  std::condition_variable notFull_;

  std::mutex drainMutex_;           ///< 配合 drainCv_ 使用，独立于 mutex_
  std::condition_variable drainCv_;
  std::atomic<size_t> pendingCount_{0};  ///< 队列中 + 正在执行的任务总数

  std::vector<std::thread> threads_;
  std::deque<Task> queue_;

  size_t maxQueueSize_{0};
  std::atomic<bool> running_{false};
  std::atomic<bool> started_{false};  ///< 防止 start() 并发重入
};

}  // namespace thread
}  // namespace pickup
