#pragma once

#include <string>

namespace pickup {
namespace utils {

/**
 * @brief 表示操作的“成功”或“失败”状态，并持有相关的错误描述信息
 * 本类用于封装操作结果，可通过检查状态码或错误信息判断操作结果。
 *
 * 示例用法：
 *  @code
 *  Result myOperation()
 *  {
 *     if (doSomeCriticalTask())
 *          return Result::ok();          // 成功返回
 *     else
 *          return Result::fail("foobar didn't work!"); // 失败返回
 *  }
 *
 *  void handleResult()
 *  {
 *      const auto result = myOperation();
 *      if (result) {  // 等价于 result.isOk()
 *          // 成功处理逻辑
 *          launchCelebrationFireworks();
 *      } else {
 *          // 失败处理逻辑
 *          logger.logError("The foobar operation failed! Error message is：" + result.getErrorMessage());
 *      }
 *  }
 */
class Result {
 public:
  /**
   * @brief 创建成功结果对象
   * @return 包含成功状态的结果对象
   * @post 对象的isOk()返回true
   * @post getErrorMessage()返回空字符串
   */
  static Result ok() noexcept { return Result(); }

  /**
   * @brief 创建失败结果对象
   * @param errorMessage 错误描述信息（UTF-8编码）
   * @return 包含失败状态的结果对象
   * @note 若错误信息为空：
   * - 自动使用"未知错误"作为默认描述
   * - 记录警告日志（仅在DEBUG模式下）
   * @warning 错误信息中不应包含敏感数据
   * @post 对象的failed()返回true
   */
  static Result fail(const std::string& errorMessage) noexcept;

  /**
   * @brief 检查操作是否成功
   * @retval true  操作成功完成
   * @retval false 操作失败，需检查错误信息
   * @see getErrorMessage()
   */
  bool isOk() const noexcept;

  /**
   * @brief 检查操作是否失败
   * @retval true  操作失败，可通过getErrorMessage()获取详情
   * @retval false 操作成功
   * @see operator!
   */
  bool failed() const noexcept;

  /**
   * @brief 布尔转换运算符（用于简化成功检查）
   * @return 与isOk()相同的结果
   * @code
   * if (result) { // 成功处理 }
   * @endcode
   */
  explicit operator bool() const noexcept;

  /**
   * @brief 逻辑非运算符（用于简化失败检查）
   * @return 与failed()相同的结果
   * @code
   * if (!result) { // 失败处理 }
   * @endcode
   */
  bool operator!() const noexcept;
  /**
   * @brief 获取错误描述信息
   * @return 包含错误描述的不可变引用：
   * - 成功时：空字符串
   * - 失败时：构造时传入的错误信息（或"未知错误"）
   *  @warning 返回的引用在对象生命周期结束后失效
   * @see failed()
  */
  const std::string& getErrorMessage() const noexcept;

  // 对象生命周期管理
  Result(const Result&);                 ///< 拷贝构造函数
  Result& operator=(const Result&);      ///< 拷贝赋值运算符
  Result(Result&&) noexcept;             ///< 移动构造函数
  Result& operator=(Result&&) noexcept;  ///< 移动赋值运算符

  /**
   * @brief 等价比较运算符
   * @param other 待比较的结果对象
   * @return 当且仅当两个结果状态相同且错误信息完全匹配时返回true
   * @note 比较规则：
   * - 两个成功状态等价
   * - 两个失败状态需错误信息逐字符相同
   */
  bool operator==(const Result& other) const noexcept;

  /**
   * @brief 不等价比较运算符
   * @param other 待比较的结果对象
   * @return operator==的相反结果
   */
  bool operator!=(const Result& other) const noexcept;

 private:
  // 禁止直接构造，必须通过ok()/fail()工厂方法
  Result() noexcept;                             ///< 成功状态构造函数
  explicit Result(const std::string&) noexcept;  ///< 失败状态构造函数

  // 禁用危险的类型转换
  operator int() const;    ///< 阻止隐式转换为整型
  operator void*() const;  ///< 阻止隐式转换为指针

  std::string errorMessage_;  ///< 错误描述存储
};

}  // namespace utils
}  // namespace pickup