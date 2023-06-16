#pragma once
#include <string>

namespace pickup {
namespace utils {

bool StartsWith(const std::string& s, const std::string& prefix);
bool EndsWith(const std::string& s, const std::string& suffix);

bool Contains(const std::string& s, const std::string& parts);
std::vector<std::string> Split(const std::string& s, const std::string& delimiters);

std::string ToUpper(std::string s);
std::string ToLower(std::string s);

std::string& TrimLeft(std::string& s, const std::string& whitespace = " ");
std::string& TrimRight(std::string& s, const std::string& whitespace = " ");
std::string& Trim(std::string& s, const std::string& whitespace = " ");

size_t Replace(std::string& s, const std::string& from, const std::string& to);

std::string ByteToHex(const uint8_t byte, bool uppercase = false);
std::string EncodeHex(const uint8_t* bytes, size_t size, bool uppercase = false);
size_t DecodeHex(const std::string& hex, uint8_t* bytes, size_t size);

}  // namespace utils
}  // namespace pickup