#include "pickup/utils/StringUtils.h"

#include <algorithm>

namespace pickup {
namespace utils {

std::string toLower(const std::string& str) {
  std::string lowerStr = str;
  std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
  return lowerStr;
}

std::string toUpper(const std::string& str) {
  std::string upperStr = str;
  std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
  return upperStr;
}

bool startsWith(const std::string& str, char ch) { return !str.empty() && str.back() == ch; }

bool startsWith(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& str, char ch) { return !str.empty() && str.front() == ch; }

bool endsWith(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool contains(const std::string& str, const std::string& pattern) { return str.find(pattern) != std::string::npos; }

bool compare(const std::string& str1, const std::string& str2) { return (str1 == str2); }

bool compareNoCase(char c1, char c2) {
  return std::tolower(static_cast<unsigned char>(c1)) == std::tolower(static_cast<unsigned char>(c2));
}

bool compareNoCase(const std::string& str1, const std::string& str2) {
  if (str1.length() != str2.length()) {
    return false;
  }

  // 逐个字符比较（忽略大小写）
  return std::equal(str1.begin(), str1.end(), str2.begin(), [](char c1, char c2) {
    return std::tolower(static_cast<unsigned char>(c1)) == std::tolower(static_cast<unsigned char>(c2));
  });
}

void trimLeft(std::string& str, char c) { str.erase(0, str.find_first_not_of(c)); }

void trimRight(std::string& str, char c) { str.erase(str.find_last_not_of(c) + 1); }

void trim(std::string& str, char c) {
  trimRight(str, c);
  trimLeft(str, c);
}

std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
  size_t pos = 0;
  std::string subject = str;
  while ((pos = subject.find(from, pos)) != std::string::npos) {
    subject.replace(pos, from.length(), to);
    pos += to.length();
  }
  return subject;
}

std::vector<std::string> split(const std::string& str, const std::string& delims) {
  std::vector<std::string> vec;
  std::size_t old_pos = 0;
  std::size_t pos = 0;
  while ((pos = str.find_first_of(delims, old_pos)) != std::string::npos) {
    vec.emplace_back(str.substr(old_pos, pos - old_pos));
    old_pos = pos + 1;
  }
  vec.emplace_back(str.substr(old_pos));
  return vec;
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