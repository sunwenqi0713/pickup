#pragma once

#include <string>

namespace pickup {
namespace utils {

/** 检查文件是否存在
 *
 * @param filePath 要检查的文件路径
 * @return 如果文件存在则返回 true，否则返回 false
 */
bool fileExists(const std::string& filePath);

/** 删除指定文件
 *
 * @param filePath 要删除的文件路径
 * @return 如果文件成功删除则返回 true；如果文件不存在或删除失败则返回 false
 */
bool removeFile(const std::string& filePath);

}  // namespace utils
}  // namespace pickup