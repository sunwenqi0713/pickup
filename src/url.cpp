#include "pickup/codec/url.h"

namespace pickup {
namespace codec {

inline unsigned char fromHex(unsigned char x) {
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

std::string url_encode(const std::string& value) {
  std::string result;
  result.reserve(value.length() * 3);  // Reserve space for efficiency

  const std::string hex_table = "0123456789ABCDEF";
  for (unsigned char ch : value) {
    if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
      result += static_cast<char>(ch);
    } else if (ch == ' ') {
      result += '+';  // Convert space to '+'
    } else {
      result += '%';
      result += hex_table[(ch >> 4) & 0x0F];  // High nibble
      result += hex_table[ch & 0x0F];         // Low nibble
    }
  }
  return result;
}

std::string url_decode(const std::string& value) {
  std::string result;
  result.reserve(value.length());  // Reserve space for efficiency

  for (size_t i = 0; i < value.length(); ++i) {
    char ch = value[i];
    if (ch == '%') {
      if (i + 2 >= value.size()) {
        result += '?';  // Incomplete escape sequence
        break;
      }

      const char hi = fromHex(value[i + 1]);
      const char lo = fromHex(value[i + 2]);
      result += static_cast<char>((hi << 4) + lo);  // Convert hex to character
      i += 2;                                       // Skip the next two characters
    } else if (ch == '+') {
      result += ' ';  // Convert '+' back to space
    } else {
      result += value[i];
    }
  }
  return result;
}

}  // namespace codec
}  // namespace pickup