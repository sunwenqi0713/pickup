#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
 * 十六进制编码器/解码器
 */
namespace pickup {
namespace codec {

/**
 * 生成给定数据的十六进制转储
 * @param data 要转换的二进制数据
 * @param length 数据长度
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @return 表示数据的十六进制编码字符串
 */
std::string encode(const uint8_t* data, size_t len, bool uppercase = true);

/**
 * 生成给定数据的十六进制转储
 * @param data 要转换的二进制数据
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @return 表示数据的十六进制编码字符串
 */
std::string encode(const std::vector<uint8_t>& data, bool uppercase = true) {
  return encode(data.data(), data.size(), uppercase);
}

/**
 * @brief 生成带分隔符的十六进制字符串
 *
 * @param data 要转换的二进制数据
 * @param len 数据长度
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @param separator 分隔符，默认为空格
 * @return std::string
 */
std::string encodeWithSeparator(const uint8_t* data, size_t len, bool uppercase = true, char separator = ' ');

/**
 * @brief 生成带分隔符的十六进制字符串
 *
 * @param data 要转换的二进制数据
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @param separator 分隔符，默认为空格
 * @return std::string
 */
std::string encodeWithSeparator(const std::vector<uint8_t>& data, bool uppercase = true, char separator = ' ') {
  return encodeWithSeparator(data.data(), data.size(), uppercase, separator);
}

/**
 * 将十六进制转储转换为二进制数据
 * @param input 要解码的十六进制字符串
 * @return 二进制数据，如果输入无效则返回空向量
 */
std::optional<std::vector<uint8_t>> decode(const std::string& input);

/**
 * @brief 将带分隔符的十六进制字符串转换为二进制数据
 *
 * @param input 要解码的十六进制字符串
 * @param separator 分隔符，默认为空格
 * @return std::optional<std::vector<uint8_t>>
 */
std::optional<std::vector<uint8_t>> decodeWithSeparator(const std::string& input, char separator = ' ');

/**
 * 将8位无符号整数转换为十六进制字符串
 * @param value 要转换的8位无符号整数
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @return 十六进制字符串
 */
std::string toHex(uint8_t value, bool uppercase = true);

/**
 * 将16位无符号整数转换为十六进制字符串
 * @param value 要转换的16位无符号整数
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @return 十六进制字符串
 */
std::string toHex(uint16_t value, bool uppercase = true);

/**
 * 将32位无符号整数转换为十六进制字符串
 * @param value 要转换的32位无符号整数
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @return 十六进制字符串
 */
std::string toHex(uint32_t value, bool uppercase = true);

/**
 * 将64位无符号整数转换为十六进制字符串
 * @param value 要转换的64位无符号整数
 * @param uppercase 是否使用大写字母（A-F），默认为true
 * @return 十六进制字符串
 */
std::string toHex(uint64_t value, bool uppercase = true);

}  // namespace codec
}  // namespace pickup
