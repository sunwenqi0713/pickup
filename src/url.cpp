#include "pickup/codec/url.h"

#include <cctype>

namespace pickup {
namespace codec {

/**
 * @brief 将十六进制字符转换为数值
 * @param x 十六进制字符 (0-9, A-F, a-f)
 * @return 对应的数值 (0-15)
 */
static unsigned char hexToValue(unsigned char x) {
  if (x >= '0' && x <= '9') {
    return x - '0';
  }
  if (x >= 'A' && x <= 'F') {
    return x - 'A' + 10;
  }
  if (x >= 'a' && x <= 'f') {
    return x - 'a' + 10;
  }
  return 0;
}

std::string urlEncode(const std::string& value) {
  std::string result;
  result.reserve(value.length() * 3);

  static const char kHexTable[] = "0123456789ABCDEF";
  for (unsigned char ch : value) {
    if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
      result += static_cast<char>(ch);
    } else if (ch == ' ') {
      result += '+';
    } else {
      result += '%';
      result += kHexTable[(ch >> 4) & 0x0F];
      result += kHexTable[ch & 0x0F];
    }
  }
  return result;
}

std::string urlDecode(const std::string& value) {
  std::string result;
  result.reserve(value.length());

  for (size_t i = 0; i < value.length(); ++i) {
    char ch = value[i];
    if (ch == '%') {
      if (i + 2 >= value.size()) {
        result += '?';  // 不完整的转义序列
        break;
      }

      const char hi = hexToValue(value[i + 1]);
      const char lo = hexToValue(value[i + 2]);
      result += static_cast<char>((hi << 4) + lo);
      i += 2;
    } else if (ch == '+') {
      result += ' ';
    } else {
      result += ch;
    }
  }
  return result;
}

}  // namespace codec
}  // namespace pickup