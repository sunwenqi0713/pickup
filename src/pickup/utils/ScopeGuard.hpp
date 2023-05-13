#pragma once

#include <type_traits>

// Based on http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0052r2.pdf
/*
void foo() {
  int* p = new int(42);
  auto guard = make_guard([&]() { delete p; });
  // ...
  // 在这里可以对 p 进行操作
  // ...
}
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
