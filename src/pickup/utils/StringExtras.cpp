#include "pickup/utils/StringExtras.h"

namespace pickup {
namespace utils {

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

std::string& ToUpper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) { return std::toupper(ch); });
  return s;
}

std::string& ToLower(std::string s) {
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

}  // namespace utils
}  // namespace pickup