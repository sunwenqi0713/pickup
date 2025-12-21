#pragma once

#include <functional>
#include <string>
#include <thread>

namespace pickup {
namespace thread {

/**
 * 获取线程ID
 * @return 线程ID
 */
long getThreadID();

/**
 * 设置线程名称
 * @param name 线程名称
 * @return 是否设置成功
 */
bool setThreadName(const std::string& name);

/**
 * 获取线程名称
 * @return 线程名称
 */
std::string getThreadName();

}  // namespace thread
}  // namespace pickup