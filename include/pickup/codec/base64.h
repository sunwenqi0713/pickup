#pragma once

#include <string>

namespace pickup {
namespace codec {
namespace base64 {

/**
 * @brief 将原始字节串编码为 Base64 字符串
 * @param input 输入数据指针
 * @param len   输入数据的字节长度
 * @return Base64 编码字符串
 */
[[nodiscard]] std::string encode(unsigned char const* input, size_t len);

/**
 * @brief 将字符串编码为 Base64 字符串
 * @param input 输入字符串
 * @return Base64 编码字符串
 */
[[nodiscard]] inline std::string encode(std::string const& input) {
  return encode(reinterpret_cast<const unsigned char*>(input.data()), input.size());
}

/**
 * @brief 将 Base64 字符串解码为原始字节串
 * @param input Base64 编码的输入字符串
 * @return 解码后的原始字节串
 */
[[nodiscard]] std::string decode(std::string const& input);

/**
 * @brief 将 URL 安全 Base64 字符串转换为标准 Base64 字符串
 * @param input URL 安全 Base64 编码字符串（使用 - 和 _ 代替 + 和 /）
 * @return 标准 Base64 编码字符串
 */
[[nodiscard]] std::string fromUrlSafe(const std::string& input);

/**
 * @brief 将标准 Base64 字符串转换为 URL 安全 Base64 字符串
 * @param input 标准 Base64 编码字符串
 * @return URL 安全 Base64 编码字符串（使用 - 和 _ 代替 + 和 /）
 */
[[nodiscard]] std::string toUrlSafe(const std::string& input);

}  // namespace base64
}  // namespace codec
}  // namespace pickup
