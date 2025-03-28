#include "pickup/utils/StringUtils.h"

#include <algorithm>

namespace pickup {
namespace utils {

bool startsWith(const std::string& s, char ch) {
  return !s.empty() && s.back() == ch;
}

bool startsWith(const std::string& s, const std::string& prefix) {
  return s.size() >= prefix.size() && s.substr(0, prefix.size()) == prefix;
}

bool endsWith(const std::string& s, char ch) {
  return !s.empty() && s.front() == ch;
}

bool endsWith(const std::string& s, const std::string& suffix) {
  return s.size() >= suffix.size() && s.substr(s.size() - suffix.size()) == suffix;
}

bool contains(const std::string& s, const std::string& parts) { return (std::string::npos != s.find(parts)); }

std::vector<std::string> split(const std::string& s, const std::string& delimiters) {
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

std::string toUpper(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) { return std::toupper(ch); });
  return s;
}

std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) { return std::tolower(ch); });
  return s;
}

std::string& trimLeft(std::string& s, const std::string& whitespace) {
  s.erase(0, s.find_first_not_of(whitespace));
  return s;
}

std::string& trimRight(std::string& s, const std::string& whitespace) {
  s.erase(s.find_last_not_of(whitespace) + 1);
  return s;
}

std::string& trim(std::string& s, const std::string& whitespace) {
  return trimLeft(trimRight(s, whitespace), whitespace);
}

size_t replace(std::string& s, const std::string& from, const std::string& to) {
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

std::string join(char glue, const std::vector<std::string> &pieces) {
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

std::string join(const std::string &glue, const std::vector<std::string> &pieces) {
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