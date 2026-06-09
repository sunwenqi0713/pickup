#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <thread>

namespace pickup {
namespace thread {

/**
 * @brief 当前线程相关的操作
 */
namespace this_thread {

/**
 * @brief 获取当前线程的 OS 线程 ID
 * @return 线程 ID
 */
long getThreadId();

/**
 * @brief 设置当前线程名称
 * @param name 线程名称
 */
void setName(const std::string& name);

/**
 * @brief 获取当前线程名称
 * @return 线程名称（获取失败时返回空字符串）
 */
std::string getName();

}  // namespace this_thread

/**
 * @brief std::thread 的封装，支持线程命名与生命周期回调
 *
 * 在构造时自动为新线程设置名称，并可选地注册启动/终止回调。
 *
 * @code
 * Thread t("worker", [](int sec) { std::this_thread::sleep_for(std::chrono::seconds(sec)); }, 5);
 * t.join();
 *
 * Thread t2("task", onStarted, onTerminated, [] { doWork(); });
 * t2.join();
 * @endcode
 */
class Thread {
 public:
  /** @brief 默认构造，不关联实际线程 */
  Thread() noexcept = default;

  Thread(Thread&&) noexcept = default;
  Thread& operator=(Thread&&) noexcept = default;

  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  /**
   * @brief 创建并启动一个命名线程
   * @tparam Function 可调用对象类型
   * @tparam Args 参数类型
   * @param name 线程名称
   * @param f    线程执行的函数
   * @param args 转发给函数的参数
   */
  template <class Function, class... Args>
  explicit Thread(std::string name, Function&& f, Args&&... args)
      : name_(std::move(name)) {
    startThread(std::bind(std::forward<Function>(f), std::forward<Args>(args)...),
                nullptr, nullptr);
  }

  /**
   * @brief 创建并启动一个命名线程（含生命周期回调）
   * @tparam Function 可调用对象类型
   * @tparam Args 参数类型
   * @param name         线程名称
   * @param onStarted    线程启动后、执行 f 前的回调（可为 nullptr）
   * @param onTerminated 线程执行结束后的回调（可为 nullptr）
   * @param f            线程执行的函数
   * @param args         转发给函数的参数
   */
  template <class Function, class... Args>
  Thread(std::string name,
         std::function<void(const std::string&)> onStarted,
         std::function<void(const std::string&)> onTerminated,
         Function&& f,
         Args&&... args)
      : name_(std::move(name)) {
    startThread(std::bind(std::forward<Function>(f), std::forward<Args>(args)...),
                std::move(onStarted), std::move(onTerminated));
  }

  ~Thread();

  /** @brief 获取底层线程的 id */
  std::thread::id getId() const noexcept;

  /** @brief 线程能否被 join */
  bool joinable() const noexcept;

  /** @brief 等待线程结束 */
  void join();

  /** @brief 分离线程 */
  void detach();

  /** @brief 交换两个 Thread 对象 */
  void swap(Thread& other) noexcept;

  /** @brief 返回硬件支持的并发线程数 */
  static size_t hardwareConcurrency() noexcept;

  /** @brief 获取线程名称 */
  const std::string& name() const noexcept { return name_; }

 private:
  void startThread(std::function<void()> task,
                   std::function<void(const std::string&)> onStarted,
                   std::function<void(const std::string&)> onTerminated);

  std::string name_;
  std::thread thread_;
};

}  // namespace thread
}  // namespace pickup
