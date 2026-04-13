#include "pickup/codec/url.h"

#include <cctype>

namespace pickup {
namespace codec {
namespace url {

// 将十六进制字符转换为数值
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

std::string encode(const std::string& input) {
  std::string result;
  result.reserve(input.length() * 3);

  static const char kHexTable[] = "0123456789ABCDEF";
  for (unsigned char ch : input) {
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

std::string decode(const std::string& input) {
  std::string result;
  result.reserve(input.length());

  for (size_t i = 0; i < input.length(); ++i) {
    char ch = input[i];
    if (ch == '%') {
      if (i + 2 >= input.size()) {
        result += '?';  // 不完整的转义序列
        break;
      }

      const char hi = hexToValue(input[i + 1]);
      const char lo = hexToValue(input[i + 2]);
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

}  // namespace url
}  // namespace codec
}  // namespace pickup
