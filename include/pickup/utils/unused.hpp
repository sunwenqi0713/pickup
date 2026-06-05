#pragma once

namespace pickup {
namespace utils {

/**
 * @brief 用于避免“未使用变量”告警的变参函数模板
 * @details 该函数接受任意数量和类型的参数，但不执行任何操作
 */
template <typename... Args>
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
constexpr
#else
inline
#endif
    void unused(const Args&...) noexcept {
}

}  // namespace utils
}  // namespace pickup

/**
 * @brief 跨平台未使用变量告警抑制宏
 * @details 根据编译器类型选择不同实现
 */

#if defined(_MSC_VER)
#define PICKUP_UNUSED(...) ((void)(__VA_ARGS__))
#else
#define PICKUP_UNUSED(...) (decltype(pickup::utils::unused(__VA_ARGS__))())
#endif

/**
 * @brief 使用示例
 *
 * 1. 抑制未使用变量警告：
 *    int unused_variable = 42;
 *    PICKUP_UNUSED(unused_variable);  // 避免编译器警告
 *
 * 2. 抑制多个未使用变量：
 *    void foo(int x, int y) {
 *        PICKUP_UNUSED(x, y);  // 同时标记x和y为"已使用"
 *    }
 *
 * 3. 在条件编译中使用：
 *    #ifdef DEBUG
 *        // 调试代码...
 *    #else
 *        int debug_only_var = get_debug_value();
 *        PICKUP_UNUSED(debug_only_var);  // 发布版本中避免警告
 *    #endif
 */