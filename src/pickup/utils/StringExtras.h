#pragma once
#include <string>

namespace pickup {
namespace utils {

bool StartsWith(const std::string& s, const std::string& prefix);
bool EndsWith(const std::string& s, const std::string& suffix);

bool Contains(const std::string& s, const std::string& parts);
std::vector<std::string> Split(const std::string& s, const std::string& delimiters);

std::string& ToUpper(std::string s);
std::string& ToLower(std::string s);

std::string& TrimLeft(std::string& s, const std::string& whitespace = " ");
std::string& TrimRight(std::string& s, const std::string& whitespace = " ");
std::string& Trim(std::string& s, const std::string& whitespace = " ");

std::size_t Replace(std::string& s, const std::string& from, const std::string& to);

}  // namespace utils
}  // namespace pickup