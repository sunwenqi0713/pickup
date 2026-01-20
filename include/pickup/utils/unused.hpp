#pragma once

namespace pickup {
namespace utils {

// 用于避免"未使用变量"警告的变参函数模板。
// 该函数接受任意数量和类型的参数，但不执行任何操作。
// 使用constexpr（如果编译器支持C++14及以上）或inline进行优化。
template <typename... Args>
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
constexpr  // C++14及以上使用constexpr，编译时求值
#else
inline  // 否则使用inline，建议编译器内联展开
#endif
    void unused(const Args&...) noexcept {  // noexcept保证不抛出异常
                                            // 函数体为空，不执行任何操作
                                            // 模板参数包Args...可以接受任意数量和类型的参数
}

}  // namespace utils
}  // namespace pickup

// 跨平台的未使用变量警告抑制宏
// 根据编译器类型选择不同的实现方式

#if defined(_MSC_VER)  // Microsoft Visual C++编译器
// 针对MSVC的宏实现：将参数转换为void表达式
// (void)表达式是MSVC中抑制未使用变量警告的标准做法
#define PICKUP_UNUSED(...) ((void)(__VA_ARGS__))
#else  // 其他编译器（GCC, Clang等）
// 针对其他编译器的宏实现：通过decltype调用unused函数
// decltype(pickup::utils::unused(__VA_ARGS__))() 创建一个函数调用表达式类型
// 然后立即调用，相当于执行 pickup::utils::unused(__VA_ARGS__)
// 这种方式能够更好地与编译器优化配合
#define PICKUP_UNUSED(...) (decltype(pickup::utils::unused(__VA_ARGS__))())
#endif

/** 使用示例：
 * 使用示例：
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