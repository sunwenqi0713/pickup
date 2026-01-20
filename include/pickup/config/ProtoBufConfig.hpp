#pragma once

#include <string>

namespace pickup {
namespace config {

template <typename ConfigType>
class ProtoBufConfig {
 public:
  virtual ~ProtoBufConfig() = default;

  const ConfigType& config() const;

  bool loadFromFile(const std::string& conf_file);

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
    std::cerr << "load protobuf config from file fail, path:" << path << std::endl;
    return false;
  }
  std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  if (!google::protobuf::TextFormat::ParseFromString(text, &config_)) {
    std::cerr << "Failed to parse text config from file: " << path << std::endl;
    return false;
  }
  return true;
}

template <typename ConfigType>
bool ProtoBufConfig<ConfigType>::dumpToFile(const std::string& path) const {
  std::ofstream file(path, std::ios::out);
  if (!file.is_open()) {
    std::cerr << "dump protobuf config to file fail, path:" << path << std::endl;
    return false;
  }

  std::string text;
  google::protobuf::TextFormat::PrintToString(config_, &text);
  file << text;
  return true;
}

}  // namespace config
}  // namespace pickup
