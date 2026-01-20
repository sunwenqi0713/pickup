#include "pickup/codec/hex.h"

#include <array>
#include <iostream>

namespace pickup {
namespace codec {

std::string encode(const uint8_t* data, size_t len, bool uppercase) {
  // 十六进制字符表
  const char* hex_table = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

  std::string hex_str;
  hex_str.reserve(len * 2);  // 预分配空间，每个字节对应2个字符

  for (size_t i = 0; i < len; ++i) {
    uint8_t byte = data[i];
    // 将高4位和低4位分别转换为十六进制字符
    hex_str.push_back(hex_table[(byte >> 4) & 0x0F]);  // 高4位
    hex_str.push_back(hex_table[byte & 0x0F]);         // 低4位
  }

  return hex_str;
}

std::string encodeWithSeparator(const uint8_t* data, size_t len, bool uppercase, char separator) {
  const char* hex_table = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

  std::string hex_str;
  if (len == 0) return hex_str;

  // 预计算内存：每个字节2字符 + 分隔符
  size_t total_len = len * 2;
  if (separator != '\0') {
    total_len += (len - 1);  // 分隔符数量为 len-1
  }
  hex_str.reserve(total_len);

  // 逐个字节处理
  for (size_t i = 0; i < len; ++i) {
    uint8_t byte = data[i];
    hex_str.push_back(hex_table[(byte >> 4) & 0x0F]);  // 高4位
    hex_str.push_back(hex_table[byte & 0x0F]);         // 低4位

    // 添加分隔符（最后一个字节不添加）
    if (separator != '\0' && i != len - 1) {
      hex_str.push_back(separator);
    }
  }

  return hex_str;
}

std::optional<std::vector<uint8_t>> decode(const std::string& input) {
  // 验证输入有效性
  if (input.length() % 2 != 0) {
    std::cerr << "Invalid hex string length: " << input.length() << ", hex string length must be even." << std::endl;
    return std::nullopt;
  }

  // 构建反向映射表（字符 -> 4位值）
  std::vector<uint8_t> lookup(256, 0xFF);  // 初始化为无效值
  for (int i = 0; i < 10; ++i) {
    lookup['0' + i] = i;  // 数字0-9
  }
  for (int i = 0; i < 6; ++i) {
    lookup['A' + i] = 10 + i;  // 大写A-F
    lookup['a' + i] = 10 + i;  // 小写a-f
  }

  std::vector<uint8_t> bytes;
  bytes.reserve(input.length() / 2);  // 预分配空间

  for (size_t i = 0; i < input.length(); i += 2) {
    // 检查字符有效性
    uint8_t high_nibble = lookup[static_cast<uint8_t>(input[i])];
    uint8_t low_nibble = lookup[static_cast<uint8_t>(input[i + 1])];

    if (high_nibble == 0xFF || low_nibble == 0xFF) {
      std::cerr << "Invalid hex character: " << input[i] << " or " << input[i + 1] << std::endl;
      return std::nullopt;
    }

    // 组合高4位和低4位
    bytes.push_back((high_nibble << 4) | low_nibble);
  }

  return bytes;
}

std::optional<std::vector<uint8_t>> decodeWithSeparator(const std::string& input, char separator) {
  std::vector<uint8_t> bytes;
  std::string clean_hex;
  clean_hex.reserve(input.length());

  // 移除分隔符
  for (char c : input) {
    if (c != separator) {
      clean_hex.push_back(c);
    }
  }

  return decode(clean_hex);
}

std::string toHex(uint8_t value, bool uppercase) {
  const char* hex_table = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
  return {hex_table[value >> 4], hex_table[value & 0x0F]};
}

std::string toHex(uint16_t value, bool uppercase) {
  std::array<uint8_t, 2> data;

  // This is explicitly done for performance reasons
  // using std::stringstream with std::hex is ~3 orders of magnitude slower.
  data[1] = (value & 0x00FF);
  data[0] = (value & 0xFF00) >> 8;

  return encode(data.data(), data.size());
}

std::string toHex(uint32_t value, bool uppercase) {
  std::array<uint8_t, 4> data;

  // This is explicitly done for performance reasons
  // using std::stringstream with std::hex is ~3 orders of magnitude slower
  data[3] = (value & 0x000000FF);
  data[2] = (value & 0x0000FF00) >> 8;
  data[1] = (value & 0x00FF0000) >> 16;
  data[0] = (value & 0xFF000000) >> 24;

  return encode(data.data(), data.size(), uppercase);
}

std::string toHex(uint64_t value, bool uppercase) {
  std::array<uint8_t, 8> data;

  // This is explicitly done for performance reasons
  data[7] = (value & 0x00000000000000FF);
  data[6] = (value & 0x000000000000FF00) >> 8;
  data[5] = (value & 0x0000000000FF0000) >> 16;
  data[4] = (value & 0x00000000FF000000) >> 24;
  data[3] = (value & 0x000000FF00000000) >> 32;
  data[2] = (value & 0x0000FF0000000000) >> 40;
  data[1] = (value & 0x00FF000000000000) >> 48;
  data[0] = (value & 0xFF00000000000000) >> 56;

  return encode(data.data(), data.size(), uppercase);
}

}  // namespace codec
}  // namespace pickup
