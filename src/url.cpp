#include "pickup/codec/url.h"

namespace pickup {
namespace codec {

std::string url_encode(const std::string& value) {
  std::string result;
  result.reserve(value.length());  // Reserve space for efficiency

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

  const std::string hex_table = "0123456789ABCDEF";
  for (size_t i = 0; i < value.length(); ++i) {
    char ch = value[i];
    if (ch == '%') {
      // Check if there are enough characters for hex decoding
      if (i + 2 >= value.size()) {
        result += '?';
        break;
      }

      const char hi = value[i + 1];
      const char lo = value[i + 2];

      auto hi_iter = std::find(hex_table.begin(), hex_table.end(), toupper(hi));
      auto lo_iter = std::find(hex_table.begin(), hex_table.end(), toupper(lo));

      if (hi_iter == hex_table.end() || lo_iter == hex_table.end()) {
        result += '?';
        break;
      }

      int hiv = (int)(hi_iter - hex_table.begin());
      int lov = (int)(lo_iter - hex_table.begin());

      result += (char)((hiv << 4) + lov);  // Convert hex to character
      i += 2;                              // Skip the next two characters
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