#pragma once

#include <functional>
#include <string>
#include <thread>

namespace pickup {
namespace thread {
/**
 * @file Thread.h
 * @brief 线程类封装
 * @note 该文件提供了一个跨平台的线程类封装，支持线程创建、命名、优先级设置等功能。
 */
class Thread {
 public:
  /**
   * @enum ThreadPriority
   * @brief 线程优先级枚举，跨平台抽象（具体实现依赖操作系统API）
   * @note 不同平台优先级映射：
   *       - Windows：使用内核级优先级（0-6）
   *       - Unix/Mac：需配合实时调度策略（SCHED_RR/SCHED_FIFO）
   *       Critical级别可能需要root/admin权限
   */
  enum ThreadPriority {
    Lowest,       ///< 最低（后台任务）
    BelowNormal,  ///< 低于正常（非关键任务）
    Normal,       ///< 默认（常规任务）
    AboveNormal,  ///< 高于正常（I/O密集型）
    Highest,      ///< 最高（实时计算）
    Critical      ///< 关键系统级（慎用）
  };

  Thread() noexcept = default;
  Thread(Thread&&) noexcept = default;

  /**
   * @brief 构造并启动线程
   * @tparam callable_type 可调用类型（函数/lambda等）
   * @param callable 线程执行主体
   * @param name 线程名称（部分系统限制长度，如Linux最长16字符）
   * @param thread_started_callback 线程启动后回调（在callable之前执行）
   * @param thread_terminated_callback 线程结束前回调（在callable之后执行）
   */
  template <class callable_type>
  Thread(callable_type&& callable, std::string name = "",
         std::function<void(const std::string& thread_name)> thread_started_callback = nullptr,
         std::function<void(const std::string& thread_name)> thread_terminated_callback = nullptr) {
    thread_ = std::thread([name = std::move(name), callable = std::forward<callable_type>(callable),
                           started_cb = std::move(thread_started_callback),
                           terminated_cb = std::move(thread_terminated_callback)]() mutable {
      set_name(name);

      if (started_cb) {
        started_cb(name);
      }

      callable();

      if (terminated_cb) {
        terminated_cb(name);
      }
    });
  }

  Thread& operator=(Thread&& rhs) noexcept = default;

  /**
   * @brief 获取线程id
   *
   * @return std::thread::id
   */
  std::thread::id get_id() const noexcept;

  /**
   * @brief 线程能否调用join
   *
   * @return true
   * @return false
   */
  bool joinable() const noexcept;

  /**
   * @brief join 线程
   *
   */
  void join();

  /**
   * @brief 返回可以并发执行的硬件线程的数量
   *
   * @return size_t
   */
  static size_t hardware_concurrency() noexcept;

  /**
   * @brief 设置线程名称
   *
   * @param name
   */
  static void set_name(const std::string& name) noexcept;

  /**
   * @brief 设置线程优先级
   *
   * @param priority
   */
  static void set_thread_priority(int priority) noexcept;

 private:
  static constexpr size_t kDefaultNumberOfCores = 8;  ///< 硬件并发数默认值（当无法获取时）
  std::thread thread_;
};

}  // namespace thread
}  // namespace pickup
