#pragma once

#include <string>
#include <vector>

namespace pickup {
namespace utils {

bool startsWith(const std::string& s, char ch);
bool startsWith(const std::string& s, const std::string& prefix);
bool endsWith(const std::string& s, char ch);
bool endsWith(const std::string& s, const std::string& suffix);

bool contains(const std::string& s, const std::string& parts);
std::vector<std::string> split(const std::string& s, const std::string& delimiters);

std::string toUpper(std::string s);
std::string toLower(std::string s);

std::string& trimLeft(std::string& s, const std::string& whitespace = " ");
std::string& trimRight(std::string& s, const std::string& whitespace = " ");
std::string& trim(std::string& s, const std::string& whitespace = " ");

size_t replace(std::string& s, const std::string& from, const std::string& to);

std::string join(char glue, const std::vector<std::string> &pieces);
std::string join(const std::string &glue, const std::vector<std::string> &pieces);

}  // namespace utils
}  // namespace pickup