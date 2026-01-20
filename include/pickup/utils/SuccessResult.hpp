#pragma once

#include "pickup/utils/Result.hpp"

namespace pickup {
namespace utils {

/**
 * @c Result 类的简化版本，假设状态只有成功或失败两种。
 *
 * 这个类专门用于只需要表示成功/失败的场景，
 * 相比通用的 Result<bool, E> 提供更语义化的接口。
 *
 * @tparam E 与结果关联的值的类型。
 */
template <typename E>
class SuccessResult : public Result<bool, E> {
 public:
  /**
   * 创建成功结果并关联一个值。
   *
   * @param value 要与结果关联的值。
   * @return 包含值的成功结果。
   */
  inline static SuccessResult<E> success(E value);

  /**
   * 创建失败结果。
   *
   * @return 失败结果（无关联值）。
   */
  inline static SuccessResult<E> failure();

  /**
   * 构造函数，同时提供成功状态和值。
   *
   * @param succeeded 成功状态。true 表示成功，false 表示失败。
   * @param value 要与结果关联的值。
   */
  inline SuccessResult(bool succeeded, E value);

  /**
   * 检查结果状态是否为成功。
   *
   * @return 如果结果状态为成功则返回 true，否则返回 false。
   */
  inline bool isSucceeded();

 protected:
  /**
   * 构造函数，仅提供成功状态。
   * 注意：此构造函数为保护成员，因为失败结果不应该包含值。
   *
   * @param succeeded 成功状态。true 表示成功，false 表示失败。
   */
  explicit inline SuccessResult(bool succeeded);
};

/**
 * 构造函数实现 - 仅状态。
 * 用于创建失败结果（失败时不包含值）。
 */
template <typename E>
SuccessResult<E>::SuccessResult(bool succeeded) : Result<bool, E>(succeeded) {}

/**
 * 构造函数实现 - 状态和值。
 * 用于创建成功结果（成功时包含值）。
 */
template <typename E>
SuccessResult<E>::SuccessResult(bool succeeded, E value) : Result<bool, E>(succeeded, value) {}

/**
 * 检查结果是否成功。
 * @return 成功状态。
 */
template <typename E>
bool SuccessResult<E>::isSucceeded() {
  return Result<bool, E>::status();  // 基类中的状态访问器
}

/**
 * 静态工厂方法 - 创建失败结果。
 * 失败结果不包含关联的值。
 */
template <typename E>
SuccessResult<E> SuccessResult<E>::failure() {
  return SuccessResult(false);  // 使用保护构造函数
}

/**
 * 静态工厂方法 - 创建成功结果。
 * 成功结果必须关联一个值。
 */
template <typename E>
SuccessResult<E> SuccessResult<E>::success(E value) {
  return SuccessResult<E>(true, value);  // 成功状态 + 值
}

}  // namespace utils
}  // namespace pickup