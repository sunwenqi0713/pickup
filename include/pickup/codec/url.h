#pragma once

#include <string>

namespace pickup {
namespace codec {

/**
 * @brief 将字符串进行URL编码
 * @param value 输入字符串
 * @return URL编码后的字符串
 */
std::string urlEncode(const std::string& value);

/**
 * @brief 将URL编码的字符串解码
 * @param value URL编码的输入字符串
 * @return 解码后的字符串
 */
std::string urlDecode(const std::string& value);

}  // namespace codec
}  // namespace pickup