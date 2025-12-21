#pragma once

#include <string>

namespace pickup {
namespace utils {

bool fileExists(const std::string& filePath);

bool removeFile(const std::string& filePath);

}
}