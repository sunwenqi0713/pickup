#pragma once

/** @brief 操作系统检测 */
#if defined(_WIN32) || defined(__CYGWIN__)
#define PICKUP_WIN_OS
#elif defined(__APPLE__) || defined(__MACH__)
#define PICKUP_MAC_OS
#elif defined(__ANDROID__)
#define PICKUP_ANDROID_OS
#elif defined(__FreeBSD__)
#define PICKUP_FREE_BSD_OS
#elif defined(unix) || defined(__unix__) || defined(__unix)
#define PICKUP_UNIX_OS
#endif

/** @brief 编译器检测 */
#if defined(__MINGW32__)
/** @brief 在 Windows 环境下单独标记 MinGW 编译器 */
#define PICKUP_MINGW_COMPILER
#elif defined(__clang__)
#define PICKUP_CLANG_COMPILER
#elif defined(__GNUC__) || defined(__GNUG__)
#define PICKUP_GCC_COMPILER
#elif defined(_MSC_VER)
#define PICKUP_MSVC_COMPILER
#endif

/** @brief 调试模式 */
#if !defined(NDEBUG) || defined(_DEBUG)
#define PICKUP_DEBUG_MODE
#endif

/** @brief 导出/导入 API */
#if defined(PICKUP_WIN_OS)
/** @brief Windows 平台使用标准 dllexport/dllimport */
#if defined(PICKUP_EXPORT_API)
#define PICKUP_API __declspec(dllexport)
#elif defined(PICKUP_IMPORT_API)
#define PICKUP_API __declspec(dllimport)
#endif
#elif (defined(PICKUP_EXPORT_API) || defined(PICKUP_IMPORT_API)) && \
    (defined(PICKUP_CLANG_COMPILER) || defined(PICKUP_GCC_COMPILER))
/** @brief 类 Unix 平台使用 GCC/Clang 的 visibility 属性 */
#define PICKUP_API __attribute__((visibility("default")))
#endif

/** @brief 确保未定义时置空 */
#ifndef PICKUP_API
#define PICKUP_API
#endif