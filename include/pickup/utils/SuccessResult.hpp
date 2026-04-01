#pragma once

#include "pickup/utils/Result.hpp"

namespace pickup {
namespace utils {

/**
 * @c Result 类的简化版本，假设状态只有成功或失败两种。
 *
 * 这个类专门用于只需要表示成功/失败的场景，
 * 相比通用的 Result<bool, Value> 提供更语义化的接口。
 *
 * @tparam Value 与结果关联的值的类型。
 */
template <typename Value>
class SuccessResult : public Result<bool, Value> {
 public:
  /**
   * 创建成功结果并关联一个值。
   *
   * @param value 要与结果关联的值。
   * @return 包含值的成功结果。
   */
  inline static SuccessResult<Value> success(Value value);

  /**
   * 创建失败结果。
   *
   * @return 失败结果（无关联值）。
   */
  inline static SuccessResult<Value> failure();

  /**
   * 构造函数，同时提供成功状态和值。
   *
   * @param succeeded 成功状态。true 表示成功，false 表示失败。
   * @param value 要与结果关联的值。
   */
  inline SuccessResult(bool succeeded, Value value);

  /**
   * 检查结果状态是否为成功。
   *
   * 覆盖基类 isSuccess()：基类以 Status{} = false 为成功判断依据，
   * 对 bool 类型语义反转，此处修正为直接返回 status()。
   *
   * @return 如果结果状态为成功则返回 true，否则返回 false。
   */
  [[nodiscard]] bool isSuccess() const { return Result<bool, Value>::status(); }

 protected:
  /**
   * 构造函数，仅提供成功状态。
   * 注意：此构造函数为保护成员，因为失败结果不应该包含值。
   *
   * @param succeeded 成功状态。true 表示成功，false 表示失败。
   */
  explicit inline SuccessResult(bool succeeded);
};

template <typename Value>
SuccessResult<Value>::SuccessResult(bool succeeded) : Result<bool, Value>(succeeded) {}

template <typename Value>
SuccessResult<Value>::SuccessResult(bool succeeded, Value value)
    : Result<bool, Value>(succeeded, std::move(value)) {}

template <typename Value>
bool SuccessResult<Value>::isSuccess() const {
  return Result<bool, Value>::status();
}

template <typename Value>
SuccessResult<Value> SuccessResult<Value>::failure() {
  return SuccessResult(false);
}

template <typename Value>
SuccessResult<Value> SuccessResult<Value>::success(Value value) {
  return SuccessResult<Value>(true, std::move(value));
}

}  // namespace utils
}  // namespace pickup
