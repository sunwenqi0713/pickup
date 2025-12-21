#pragma once

#include <type_traits>

// Based on http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0052r2.pdf
/**
 * void foo() {
 *  int* p = new int(42);
 *  auto guard = make_guard([&]() { delete p; });
 *  // ...
 *  // 在这里可以对 p 进行操作
 *  // ...
 * }
 */

namespace pickup {
namespace utils {

template <typename F>
class ScopeGuard final {
 public:
  explicit ScopeGuard(F&& exit_func) : exit_func_(std::forward<F>(exit_func)) {}

  ScopeGuard(ScopeGuard&& other)
      : exit_func_(std::move(other.exit_func_)), execute_(std::exchange(other.execute_, false)) {}

  ~ScopeGuard() {
    if (execute_ && exit_func_ != nullptr) exit_func_();
  }

  void release() { execute_ = false; }

  ScopeGuard(const ScopeGuard&) = delete;
  ScopeGuard& operator=(const ScopeGuard&) = delete;
  ScopeGuard& operator=(ScopeGuard&&) = delete;

 private:
  F exit_func_;
  bool execute_{true};
};

template <typename F>
ScopeGuard<F> make_guard(F&& f) {
  return ScopeGuard<F>(std::forward<F>(f));
}

}  // namespace utils
}  // namespace pickup

// 便利宏
#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

/**
 * @brief 创建作用域保护对象，自动执行清理函数
 * @param cleanup_expr 清理表达式（lambda 或函数）
 *
 * 示例：
 *   SCOPE_GUARD([&]() { delete ptr; });
 *   SCOPE_GUARD([&]() { fclose(file); });
 */
#define SCOPE_GUARD(cleanup_expr) auto CONCAT(scope_guard_, __LINE__) = pickup::utils::make_guard(cleanup_expr)

/**
 * @brief 创建具名作用域保护对象，可通过名称释放
 * @param name 保护对象名称
 * @param cleanup_expr 清理表达式
 *
 * 示例：
 *   NAMED_SCOPE_GUARD(guard, [&]() { resource.release(); });
 *   // ... 某些条件下提前释放
 *   guard.release();
 */
#define NAMED_SCOPE_GUARD(name, cleanup_expr) auto name = pickup::utils::make_guard(cleanup_expr)