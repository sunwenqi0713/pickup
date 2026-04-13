#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace pickup {
namespace codec {
namespace hex {

/**
 * @brief 将二进制数据编码为十六进制字符串
 * @param data      要转换的二进制数据
 * @param len       数据长度
 * @param uppercase 是否使用大写字母（A-F），默认为 true
 * @return 十六进制编码字符串
 */
[[nodiscard]] std::string encode(const uint8_t* data, size_t len, bool uppercase = true);

/**
 * @brief 将二进制数据编码为十六进制字符串（vector 重载）
 * @param data      要转换的二进制数据
 * @param uppercase 是否使用大写字母（A-F），默认为 true
 * @return 十六进制编码字符串
 */
[[nodiscard]] inline std::string encode(const std::vector<uint8_t>& data, bool uppercase = true) {
  return encode(data.data(), data.size(), uppercase);
}

/**
 * @brief 将整数编码为定宽十六进制字符串
 * @param value     要转换的值
 * @param uppercase 是否使用大写字母，默认为 true
 * @return 十六进制字符串（uint8_t→2位，uint16_t→4位，uint32_t→8位，uint64_t→16位）
 */
[[nodiscard]] std::string encode(uint8_t  value, bool uppercase = true);
[[nodiscard]] std::string encode(uint16_t value, bool uppercase = true);
[[nodiscard]] std::string encode(uint32_t value, bool uppercase = true);
[[nodiscard]] std::string encode(uint64_t value, bool uppercase = true);

/**
 * @brief 将二进制数据编码为带分隔符的十六进制字符串
 * @param data      要转换的二进制数据
 * @param len       数据长度
 * @param uppercase 是否使用大写字母（A-F），默认为 true
 * @param separator 字节间分隔符，默认为空格
 * @return 带分隔符的十六进制字符串
 */
[[nodiscard]] std::string encodeWithSeparator(const uint8_t* data, size_t len, bool uppercase = true,
                                              char separator = ' ');

/**
 * @brief 将二进制数据编码为带分隔符的十六进制字符串（vector 重载）
 * @param data      要转换的二进制数据
 * @param uppercase 是否使用大写字母（A-F），默认为 true
 * @param separator 字节间分隔符，默认为空格
 * @return 带分隔符的十六进制字符串
 */
[[nodiscard]] inline std::string encodeWithSeparator(const std::vector<uint8_t>& data, bool uppercase = true,
                                                     char separator = ' ') {
  return encodeWithSeparator(data.data(), data.size(), uppercase, separator);
}

/**
 * @brief 将十六进制字符串解码为二进制数据
 * @param input 要解码的十六进制字符串
 * @return 二进制数据，输入无效时返回 std::nullopt
 */
[[nodiscard]] std::optional<std::vector<uint8_t>> decode(const std::string& input);

/**
 * @brief 将带分隔符的十六进制字符串解码为二进制数据
 * @param input     要解码的十六进制字符串
 * @param separator 字节间分隔符，默认为空格
 * @return 二进制数据，输入无效时返回 std::nullopt
 */
[[nodiscard]] std::optional<std::vector<uint8_t>> decodeWithSeparator(const std::string& input,
                                                                       char separator = ' ');

}  // namespace hex
}  // namespace codec
}  // namespace pickup
