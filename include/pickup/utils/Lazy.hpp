#pragma once

#include <functional>
#include <mutex>
#include <optional>

namespace pickup {
namespace utils {

/**
 * @brief 线程安全的延迟初始化容器
 * @tparam T 存储的值类型
 *
 * 用于按需创建对象，避免不必要的开销。示例：
 * @code
 * Lazy<ExpensiveObject> lazy([]() { return ExpensiveObject(); });
 * const auto& obj = lazy.getOrCreate();  // 首次访问时创建
 * @endcode
 */
template <typename T>
class Lazy {
 public:
  using Factory = std::function<T()>;

  explicit Lazy() = default;
  explicit Lazy(Factory factory) : factory_(std::move(factory)) {}
  ~Lazy() = default;

  /**
   * @brief 获取或创建值
   * @return 值的常量引用
   * @note 首次调用时通过工厂函数创建，后续调用直接返回缓存值
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
   * @brief 获取已创建的值（如果存在）
   * @return 值的 optional 引用，未创建时为 nullopt
   */
  const std::optional<T>& getIfCreated() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return value_;
  }

  /**
   * @brief 检查值是否已创建
   * @return 已创建返回 true，否则返回 false
   */
  bool hasValue() const {
    std::lock_guard<std::mutex> guard(mutex_);
    return value_.has_value();
  }

  /**
   * @brief 设置新的工厂函数
   * @param factory 新的工厂函数
   */
  void setFactory(Factory&& factory) {
    std::lock_guard<std::mutex> guard(mutex_);
    factory_ = std::move(factory);
  }

 private:
  mutable std::mutex mutex_;
  Factory factory_;
  std::optional<T> value_;
};

}  // namespace utils
}  // namespace pickup
