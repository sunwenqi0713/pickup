#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace pickup {
namespace encoding {

// 将字节数组转换为十六进制字符串
std::string bytesToHex(const uint8_t* data, size_t len, bool uppercase = true);
// 重载版本：支持直接传入 std::vector<uint8_t>
std::string bytesToHex(const std::vector<uint8_t>& data, bool uppercase = true);
// 支持分隔符
std::string bytesToHexWithSeparator(const uint8_t* data, size_t len, bool uppercase = true, char separator = ' ');
// 重载版本：支持直接传入 std::vector<uint8_t>
std::string bytesToHexWithSeparator(const std::vector<uint8_t>& data, bool uppercase = true, char separator = ' ');

// 将十六进制字符串转换为字节数组
std::optional<std::vector<uint8_t>> hexToBytes(const std::string& hex_str);
// 支持分隔符
std::optional<std::vector<uint8_t>> hexToBytesWithSeparator(const std::string& hex_str, char separator = ' ');

}  // namespace encoding
}  // namespace pickup
