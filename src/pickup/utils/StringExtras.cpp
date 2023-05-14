#include "pickup/utils/StringExtras.h"

namespace pickup {
namespace utils {

namespace detail {

enum HexConversionMode { Lowercase, Uppercase };

inline const char* HexDigitsForMode(HexConversionMode mode) {
  static const char lowercaseHexDigits[] = "0123456789abcdef";
  static const char uppercaseHexDigits[] = "0123456789ABCDEF";
  return mode == Lowercase ? lowercaseHexDigits : uppercaseHexDigits;
}

inline int HexCharToDecimal(char ch) {
  if (ch >= '0' && ch <= '9') return (ch - '0');
  if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 10);
  if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 10);
  return -1;
}

}  // namespace detail

bool StartsWith(const std::string& s, const std::string& prefix) {
  return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

bool EndsWith(const std::string& s, const std::string& suffix) {
  return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}

bool Contains(const std::string& s, const std::string& parts) { return (std::string::npos != s.find(parts)); }

std::vector<std::string> Split(const std::string& s, const std::string& delimiters) {
  std::vector<std::string> tokens;
  std::string::size_type last_pos = s.find_first_not_of(delimiters, 0);
  std::string::size_type pos = s.find_first_of(delimiters, last_pos);
  while (std::string::npos != pos || std::string::npos != last_pos) {
    tokens.emplace_back(s.substr(last_pos, pos - last_pos));
    last_pos = s.find_first_not_of(delimiters, pos);
    pos = s.find_first_of(delimiters, last_pos);
  }

  return tokens;
}

std::string ToUpper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) { return std::toupper(ch); });
  return s;
}

std::string ToLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) { return std::tolower(ch); });
  return s;
}

std::string& TrimLeft(std::string& s, const std::string& whitespace) {
  s.erase(0, s.find_first_not_of(whitespace));
  return s;
}

std::string& TrimRight(std::string& s, const std::string& whitespace) {
  s.erase(s.find_last_not_of(whitespace) + 1);
  return s;
}

std::string& Trim(std::string& s, const std::string& whitespace) {
  return TrimLeft(TrimRight(s, whitespace), whitespace);
}

std::size_t Replace(std::string& s, const std::string& from, const std::string& to) {
  if (from.empty() || s.empty()) {
    return 0;
  }

  std::size_t count = 0;
  std::size_t pos = s.find(from);
  while (pos != std::string::npos) {
    ++count;
    s.replace(pos, from.length(), to);
    pos = s.find(from);
  }

  return count;
}

std::string ByteToHex(const uint8_t value, bool uppercase) {
  auto digits = detail::HexDigitsForMode(uppercase ? detail::Uppercase : detail::Lowercase);
  uint8_t high = static_cast<uint8_t>(value >> 4);
  uint8_t low = static_cast<uint8_t>(value & 0x0F);
  return {digits[high], digits[low]};
}

std::string EncodeHex(const uint8_t* bytes, size_t length, bool uppercase) {
  std::string result;
  result.reserve(length * 2);
  for (std::size_t i = 0; i < length; ++i) {
    auto value = static_cast<uint8_t>(bytes[i]);
    result.append(ByteToHex(value, uppercase));
  }
  return result;
}

std::size_t DecodeHex(const std::string& from, uint8_t* to, size_t length) {
  if (from.length() % 2 != 0) {
    return 0;
  }

  std::size_t count = from.length() / 2;
  if (count > length) {
    return 0;
  }

  for (int i = 0; i != from.length(); i += 2) {
    auto value = (detail::HexCharToDecimal(from[i]) << 4) | detail::HexCharToDecimal(from[i + 1]);
    if (!(value >= 0 && value <= 255)) {
      return 0;
    }
    *to++ = value;
  }

  return count;
}

}  // namespace utils
}  // namespace pickup