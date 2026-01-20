#pragma once

#include <functional>
#include <string>
#include <thread>

namespace pickup {
namespace thread {

/**
 * @brief 获取当前线程ID
 * @return 线程ID
 */
long getThreadID();

/**
 * @brief 设置当前线程名称
 * @param name 线程名称
 * @return 设置成功返回 true，否则返回 false
 */
bool setThreadName(const std::string& name);

/**
 * @brief 获取当前线程名称
 * @return 线程名称
 */
std::string getThreadName();

}  // namespace thread
}  // namespace pickup