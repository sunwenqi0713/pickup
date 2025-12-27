#pragma once

#include <functional>
#include <mutex>
#include <optional>

/**
 * @brief 延迟初始化模板类
 * 用于按需创建对象，避免不必要的开销
 * Lazy<ExpensiveObject> lazy([]() {
 *   return ExpensiveObject();
 * });
 * // 首次访问时创建
 * const auto& obj = lazy.getOrCreate();
 * // 检查是否已初始化
 * if (lazy.hasValue()) {
 *     // 使用已创建的对象
 * }
 */
namespace pickup {
namespace utils {

template <typename T>
class Lazy {
 public:
  using Factory = std::function<T()>;

  explicit Lazy() = default;
  explicit Lazy(Factory factory) : factory_(std::move(factory)) {}
  ~Lazy() = default;

  /**
   * 获取或创建值
   * 如果值从未创建，则通过工厂函数创建
   * 注意：工厂函数调用后会被清空，确保单次创建
   */
  const T& getOrCreate() {
    std::lock_guard<std::mutex> guard(mutex_);
    if (!value_) {
      value_ = {factory_()};
      factory_ = Factory();
    }

    return value_.value();
  }

  /**
   * 获取已创建的值（如果存在）
   * 返回optional，未创建时返回nullopt
   */
  const std::optional<T>& getIfCreated() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return value_;
  }

  // 检查值是否已创建
  bool hasValue() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return value_.has_value();
  }

  // 设置新的工厂函数（可替换原有工厂）
  void setFactory(Factory&& factory) {
    std::lock_guard<std::mutex> guard(mutex_);
    factory_ = std::move(factory);
  }

 private:
  mutable std::mutex mutex_;  // 可变互斥锁（const成员函数也需加锁）
  Factory factory_;           // 工厂函数（创建后置空）
  std::optional<T> value_;    // 存储的值（optional表示可空状态）
};

}  // namespace utils
}  // namespace pickup
