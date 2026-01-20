#pragma once

#include <string>

namespace pickup {
namespace utils {

/**
 * @brief 检查文件是否存在
 * @param filePath 要检查的文件路径
 * @return 文件存在返回 true，否则返回 false
 */
bool fileExists(const std::string& filePath);

/**
 * @brief 删除指定文件
 * @param filePath 要删除的文件路径
 * @return 删除成功返回 true，否则返回 false
 */
bool removeFile(const std::string& filePath);

}  // namespace utils
}  // namespace pickup