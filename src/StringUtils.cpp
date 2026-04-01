#include "pickup/utils/StringUtils.h"

#include <algorithm>

namespace pickup {
namespace utils {

std::string toUpper(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return result;
}

std::string toLower(const std::string& str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return result;
}

bool isBlank(const std::string& str) {
  return str.find_first_not_of(" \t\n\r\f\v") == std::string::npos;
}

bool contains(const std::string& str, const std::string& pattern) {
  return str.find(pattern) != std::string::npos;
}

bool startsWith(const std::string& str, char ch) {
  return !str.empty() && str.front() == ch;
}

bool startsWith(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& str, char ch) {
  return !str.empty() && str.back() == ch;
}

bool endsWith(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool compareNoCase(char c1, char c2) {
  return std::tolower(static_cast<unsigned char>(c1)) ==
         std::tolower(static_cast<unsigned char>(c2));
}

bool compareNoCase(const std::string& str1, const std::string& str2) {
  if (str1.length() != str2.length()) return false;
  return std::equal(str1.begin(), str1.end(), str2.begin(), [](char c1, char c2) {
    return std::tolower(static_cast<unsigned char>(c1)) ==
           std::tolower(static_cast<unsigned char>(c2));
  });
}

void trimLeft(std::string& str, char c) {
  str.erase(0, str.find_first_not_of(c));
}

void trimRight(std::string& str, char c) {
  str.erase(str.find_last_not_of(c) + 1);
}

void trim(std::string& str, char c) {
  trimRight(str, c);
  trimLeft(str, c);
}

void trimLeft(std::string& str, const std::string& chars) {
  str.erase(0, str.find_first_not_of(chars));
}

void trimRight(std::string& str, const std::string& chars) {
  const auto pos = str.find_last_not_of(chars);
  if (pos == std::string::npos) {
    str.clear();
  } else {
    str.erase(pos + 1);
  }
}

void trim(std::string& str, const std::string& chars) {
  trimRight(str, chars);
  trimLeft(str, chars);
}

std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
  std::string result = str;
  size_t pos = 0;
  while ((pos = result.find(from, pos)) != std::string::npos) {
    result.replace(pos, from.length(), to);
    pos += to.length();
  }
  return result;
}

std::string stripPrefix(const std::string& str, const std::string& prefix) {
  if (startsWith(str, prefix)) return str.substr(prefix.size());
  return str;
}

std::string stripSuffix(const std::string& str, const std::string& suffix) {
  if (endsWith(str, suffix)) return str.substr(0, str.size() - suffix.size());
  return str;
}

std::string padLeft(const std::string& str, size_t width, char fill) {
  if (str.size() >= width) return str;
  return std::string(width - str.size(), fill) + str;
}

std::string padRight(const std::string& str, size_t width, char fill) {
  if (str.size() >= width) return str;
  return str + std::string(width - str.size(), fill);
}

std::string repeat(const std::string& str, size_t n) {
  std::string result;
  result.reserve(str.size() * n);
  for (size_t i = 0; i < n; ++i) result += str;
  return result;
}

std::vector<std::string> split(const std::string& str, const std::string& delims) {
  std::vector<std::string> vec;
  size_t old_pos = 0;
  size_t pos = 0;
  while ((pos = str.find_first_of(delims, old_pos)) != std::string::npos) {
    vec.emplace_back(str.substr(old_pos, pos - old_pos));
    old_pos = pos + 1;
  }
  vec.emplace_back(str.substr(old_pos));
  return vec;
}

std::vector<std::string> splitBy(const std::string& str, const std::string& delimiter) {
  std::vector<std::string> vec;
  if (delimiter.empty()) {
    for (char c : str) vec.emplace_back(1, c);
    return vec;
  }
  size_t start = 0;
  size_t pos = 0;
  while ((pos = str.find(delimiter, start)) != std::string::npos) {
    vec.emplace_back(str.substr(start, pos - start));
    start = pos + delimiter.size();
  }
  vec.emplace_back(str.substr(start));
  return vec;
}

std::string join(char glue, const std::vector<std::string>& pieces) {
  std::string result;
  if (!pieces.empty()) {
    result.append(pieces[0]);
    for (size_t i = 1; i < pieces.size(); ++i) {
      result.push_back(glue);
      result.append(pieces[i]);
    }
  }
  return result;
}

std::string join(const std::string& glue, const std::vector<std::string>& pieces) {
  std::string result;
  if (!pieces.empty()) {
    result.append(pieces[0]);
    for (size_t i = 1; i < pieces.size(); ++i) {
      result.append(glue);
      result.append(pieces[i]);
    }
  }
  return result;
}

}  // namespace utils
}  // namespace pickup
