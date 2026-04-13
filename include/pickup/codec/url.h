#pragma once

#include <string>

namespace pickup {
namespace codec {
namespace url {

/**
 * @brief 将字符串进行 URL 编码
 * @param input 输入字符串
 * @return URL 编码后的字符串
 */
[[nodiscard]] std::string encode(const std::string& input);

/**
 * @brief 将 URL 编码的字符串解码
 * @param input URL 编码的输入字符串
 * @return 解码后的字符串
 */
[[nodiscard]] std::string decode(const std::string& input);

}  // namespace url
}  // namespace codec
}  // namespace pickup
