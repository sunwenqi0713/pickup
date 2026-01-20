#include "pickup/Utils/FileUtils.h"

#include <cstdio>
#include <fstream>
#include <iostream>

namespace pickup {
namespace utils {

bool fileExists(const std::string& filePath) {
  std::ifstream is(filePath);
  return is.good();
}

bool removeFile(const std::string& filePath) {
  if (!fileExists(filePath)) {
    std::cout << "File does not exist: " << filePath << std::endl;
    return false;
  }

  int result = std::remove(filePath.c_str());
  if (result != 0) {
    std::cout << "Failed to remove file: " << filePath << std::endl;
    return false;
  }

  return true;
}

}  // namespace utils
}  // namespace pickup