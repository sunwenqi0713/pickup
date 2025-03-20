#pragma once

#include <functional>
#include <string>
#include <thread>

namespace pickup {
namespace thread {
/**
 * @brief 对std::thread的封装
 *
 */
class Thread {
 public:
  /**
   * @brief 默认构造
   *
   */
  Thread() noexcept = default;
  /**
   * @brief 移动构造
   *
   */
  Thread(Thread&&) noexcept = default;

  /**
   * @brief 新建一个线程
   *
   * @tparam callable_type 可执行的函数类型
   * @param callable 线程要执行的函数
   * @param name 线程名称
   * @param thread_started_callback 线程执行前调用的函数
   * @param thread_terminated_callback 线程结束后调用的函数
   */
  template <class callable_type>
  Thread(callable_type&& callable, std::string name = "",
         std::function<void(const std::string& thread_name)> thread_started_callback = nullptr,
         std::function<void(const std::string& thread_name)> thread_terminated_callback = nullptr) {
    thread_ = std::thread([name = std::move(name), callable = std::forward<callable_type>(callable),
                           thread_started_callback = std::move(thread_started_callback),
                           thread_terminated_callback = std::move(thread_terminated_callback)]() mutable {
      set_name(name);

      if (static_cast<bool>(thread_started_callback)) {
        thread_started_callback(name);
      }

      callable();

      if (static_cast<bool>(thread_terminated_callback)) {
        thread_terminated_callback(name);
      }
    });
  }

  /**
   * @brief 移动赋值
   *
   * @param rhs
   * @return Thread&
   */
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

 private:
  static constexpr size_t kDefaultNumberOfCores = 8;
  std::thread thread_;
};

}  // namespace thread
}  // namespace pickup
