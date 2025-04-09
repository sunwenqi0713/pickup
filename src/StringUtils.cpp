#include "pickup/utils/StringUtils.h"

#include <algorithm>

namespace pickup {
namespace utils {

bool startsWith(const std::string& str, char ch) { return !str.empty() && str.back() == ch; }

bool startsWith(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
}

bool endsWith(const std::string& str, char ch) { return !str.empty() && str.front() == ch; }

bool endsWith(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}

bool contains(const std::string& str, const std::string& parts) { return (std::string::npos != str.find(parts)); }

std::string toUpper(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return std::toupper(ch); });
  return str;
}

std::string toLower(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return std::tolower(ch); });
  return str;
}

bool compare(const std::string& str1, const std::string& str2) { return (str1 == str2); }

bool compareNoCase(const std::string& str1, const std::string& str2) {
  if (str1.size() != str2.size()) return false;
  return std::equal(
      str1.cbegin(), str1.cend(), str2.cbegin(),
      [](std::string::value_type l, std::string::value_type r) { return std::tolower(l) == std::tolower(r); });
}

std::string& trimLeft(std::string& str, const std::string& whitespace) {
  str.erase(0, str.find_first_not_of(whitespace));
  return str;
}

std::string& trimRight(std::string& str, const std::string& whitespace) {
  str.erase(str.find_last_not_of(whitespace) + 1);
  return str;
}

std::string& trim(std::string& str, const std::string& whitespace) {
  return trimLeft(trimRight(str, whitespace), whitespace);
}

bool replaceFirst(std::string& str, const std::string& from, const std::string& to) {
  size_t pos = str.find(from);
  if (pos == std::string::npos) return false;

  str.replace(pos, from.size(), to);
  return true;
}

bool replaceLast(std::string& str, const std::string& from, const std::string& to) {
  size_t pos = str.rfind(from);
  if (pos == std::string::npos) return false;

  str.replace(pos, from.size(), to);
  return true;
}

size_t replaceAll(std::string& str, const std::string& from, const std::string& to) {
  std::size_t count = 0;

  size_t pos = 0;
  while ((pos = str.find(from, pos)) != std::string::npos) {
    str.replace(pos, from.size(), to);
    pos += to.size();  // Move past the new string
    ++count;
  }

  return count;
}

std::vector<std::string> split(const std::string& str, const std::string& delimiters) {
  std::vector<std::string> tokens;
  std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, last_pos);
  while (std::string::npos != pos || std::string::npos != last_pos) {
    tokens.emplace_back(str.substr(last_pos, pos - last_pos));
    last_pos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, last_pos);
  }

  return tokens;
}

std::string join(char glue, const std::vector<std::string>& pieces) {
  std::string s;
  if (!pieces.empty()) {
    s.append(pieces[0]);
    for (size_t i = 1; i < pieces.size(); ++i) {
      s.push_back(glue);
      s.append(pieces[i]);
    }
  }
  return s;
}

std::string join(const std::string& glue, const std::vector<std::string>& pieces) {
  std::string s;
  if (!pieces.empty()) {
    s.append(pieces[0]);
    for (size_t i = 1; i < pieces.size(); ++i) {
      s.append(glue);
      s.append(pieces[i]);
    }
  }
  return s;
}

}  // namespace utils
}  // namespace pickup