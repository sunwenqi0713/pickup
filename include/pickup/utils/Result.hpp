#pragma once

#include <utility>

namespace pickup {
namespace utils {

/**
 * 表示操作结果的模板类。包含状态值和返回的值。
 * 提供类型安全的错误处理和值返回。
 *
 * @tparam T 状态类型，通常为枚举类型
 * @tparam E 返回值类型，当操作成功时包含有效值
 */
template <typename T, typename E>
class Result {
public:
    /**
     * 使用状态和值构造Result对象（完美转发）
     *
     * @param status 操作状态
     * @param value 返回值，将使用完美转发构造
     */
    template <typename ValueType>
    inline Result(T status, ValueType&& value);
    
    /**
     * 仅使用状态构造Result对象（失败情况）
     *
     * @param status 操作状态
     */
    inline explicit Result(T status);
    
    /**
     * 获取操作状态
     *
     * @return 操作状态
     */
    inline T status() const;
    
    /**
     * 检查操作是否成功
     *
     * @return 如果状态表示成功则返回true
     */
    inline bool isSuccess() const;
    
    /**
     * 获取返回值的引用（仅当操作成功时安全）
     *
     * @return 返回值的引用
     */
    inline E& value();
    
    /**
     * 获取返回值的常量引用（仅当操作成功时安全）
     *
     * @return 返回值的常量引用
     */
    inline const E& value() const;
    
    /**
     * 安全获取返回值，失败时返回默认值
     *
     * @param defaultValue 失败时返回的默认值
     * @return 成功时返回值，失败时返回默认值
     */
    template <typename DefaultType>
    inline E valueOr(DefaultType&& defaultValue) const;

private:
    /// 操作状态
    T status_;
    
    /// 返回值（可能无效，取决于状态）
    E value_;
};

template <typename T, typename E>
template <typename ValueType>
Result<T, E>::Result(T status, ValueType&& value) 
    : status_{status}, value_{std::forward<ValueType>(value)} {
}

template <typename T, typename E>
Result<T, E>::Result(T status) 
    : status_{status}, value_{} {
}

template <typename T, typename E>
T Result<T, E>::status() const {
    return status_;
}

template <typename T, typename E>
bool Result<T, E>::isSuccess() const {
    // 假设状态0表示成功（适用于大多数枚举）
    // 在实际使用中，可以根据需要特化此方法
    return status_ == T{};
}

template <typename T, typename E>
E& Result<T, E>::value() {
    return value_;
}

template <typename T, typename E>
const E& Result<T, E>::value() const {
    return value_;
}

template <typename T, typename E>
template <typename DefaultType>
E Result<T, E>::valueOr(DefaultType&& defaultValue) const {
    return isSuccess() ? value_ : static_cast<E>(std::forward<DefaultType>(defaultValue));
}

}  // namespace utils
}  // namespace pickup