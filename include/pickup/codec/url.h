#pragma once

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>

namespace pickup {
namespace codec {

// Encodes a string to be URL-safe
std::string url_encode(const std::string& value);

// Decodes a URL-encoded string
std::string url_decode(const std::string& value);

}  // namespace codec
}  // namespace pickup