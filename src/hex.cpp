#include "pickup/encoding/hex.h"
#include <stdexcept>

namespace pickup {
namespace encoding {

std::string bytesToHex(const uint8_t* data, size_t len, bool uppercase) {
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

std::string bytesToHex(const std::vector<uint8_t>& data, bool uppercase) {
  return bytesToHex(data.data(), data.size(), uppercase);
}

std::string bytesToHexWithSeparator(const uint8_t* data, size_t len, bool uppercase, char separator) {
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

std::string bytesToHexWithSeparator(const std::vector<uint8_t>& data, bool uppercase, char separator) {
  return bytesToHexWithSeparator(data.data(), data.size(), uppercase, separator);
}

std::vector<uint8_t> hexToBytes(const std::string& hex_str) {
  // 验证输入有效性
  if (hex_str.length() % 2 != 0) {
    throw std::invalid_argument("Hex string length must be even");
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
  bytes.reserve(hex_str.length() / 2);  // 预分配空间

  for (size_t i = 0; i < hex_str.length(); i += 2) {
    // 检查字符有效性
    uint8_t high_nibble = lookup[static_cast<uint8_t>(hex_str[i])];
    uint8_t low_nibble = lookup[static_cast<uint8_t>(hex_str[i + 1])];

    if (high_nibble == 0xFF || low_nibble == 0xFF) {
      throw std::invalid_argument("Invalid hex character");
    }

    // 组合高4位和低4位
    bytes.push_back((high_nibble << 4) | low_nibble);
  }

  return bytes;
}

std::vector<uint8_t> hexToBytesWithSeparator(const std::string& hex_str, char separator) {
  std::vector<uint8_t> bytes;
  std::string clean_hex;
  clean_hex.reserve(hex_str.length());

  // 移除分隔符
  for (char c : hex_str) {
    if (c != separator) {
      clean_hex.push_back(c);
    }
  }

  return hexToBytes(clean_hex);
}

}  // namespace encoding
}  // namespace pickup
