#pragma once

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include <google/protobuf/text_format.h>

namespace pickup {
namespace config {

template <typename ConfigType>
class ProtoBufConfig {
 public:
  virtual ~ProtoBufConfig() = default;

  const ConfigType& config() const;

  bool loadFromFile(const std::string& conf_file);

  bool loadFromString(const std::string& content);

  bool dumpToFile(const std::string& dump_file) const;

 private:
  ConfigType config_;
};

template <typename ConfigType>
const ConfigType& ProtoBufConfig<ConfigType>::config() const {
  return config_;
}

template <typename ConfigType>
bool ProtoBufConfig<ConfigType>::loadFromFile(const std::string& path) {
  std::ifstream file(path, std::ios::in);
  if (!file.is_open()) {
    return false;
  }
  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return loadFromString(content);
}

template <typename ConfigType>
bool ProtoBufConfig<ConfigType>::loadFromString(const std::string& content) {
  ConfigType tmp;
  if (!google::protobuf::TextFormat::ParseFromString(content, &tmp)) {
    return false;
  }
  config_ = std::move(tmp);
  return true;
}

template <typename ConfigType>
bool ProtoBufConfig<ConfigType>::dumpToFile(const std::string& path) const {
  std::ofstream file(path, std::ios::out);
  if (!file.is_open()) {
    return false;
  }

  std::string text;
  if (!google::protobuf::TextFormat::PrintToString(config_, &text)) {
    return false;
  }
  file << text;
  return file.good();
}

}  // namespace config
}  // namespace pickup
