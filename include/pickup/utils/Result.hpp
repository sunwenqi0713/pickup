#pragma once

#include <utility>

namespace pickup {
namespace utils {

/**
 * @brief 携带状态码和返回值的操作结果模板类
 *
 * @tparam Status 状态类型，通常为枚举。**约定：Status{} （零值/默认构造）表示成功**，
 *                其余值表示各种错误码。使用整数或枚举时须确保 0 映射到成功状态。
 * @tparam Value  返回值类型，操作成功时包含有效值；失败时值为默认初始化，不应访问。
 *
 * 示例：
 * @code
 * enum class StatusCode { Ok = 0, NotFound, IoError };
 * Result<StatusCode, std::string> r(StatusCode::Ok, "hello");
 * if (r.isSuccess()) use(r.value());
 * @endcode
 */
template <typename Status, typename Value>
class Result {
 public:
  /**
   * @brief 用状态和值构造（完美转发）
   * @param status 操作状态
   * @param value  返回值
   */
  template <typename ValueType>
  Result(Status status, ValueType&& value)
      : status_{status}, value_{std::forward<ValueType>(value)} {}

  /**
   * @brief 仅用状态构造（失败情况，value 默认初始化）
   * @param status 操作状态
   */
  explicit Result(Status status) : status_{status}, value_{} {}

  /** @brief 返回操作状态 */
  [[nodiscard]] Status status() const { return status_; }

  /** @brief 若状态等于 Status{} 则视为成功，返回 true */
  [[nodiscard]] bool isSuccess() const { return status_ == Status{}; }

  /** @brief 返回返回值的引用（仅在成功时安全调用） */
  [[nodiscard]] Value& value() { return value_; }

  /** @brief 返回返回值的常量引用（仅在成功时安全调用） */
  [[nodiscard]] const Value& value() const { return value_; }

  /**
   * @brief 安全获取返回值，失败时返回默认值
   * @param defaultValue 失败时返回的默认值
   * @return 成功时返回 value()，失败时返回 defaultValue
   */
  template <typename DefaultType>
  [[nodiscard]] Value valueOr(DefaultType&& defaultValue) const {
    return isSuccess() ? value_ : static_cast<Value>(std::forward<DefaultType>(defaultValue));
  }

 private:
  Status status_;  ///< 操作状态
  Value value_;    ///< 返回值（仅状态成功时有效）
};

}  // namespace utils
}  // namespace pickup
