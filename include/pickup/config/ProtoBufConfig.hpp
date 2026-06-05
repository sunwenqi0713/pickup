#pragma once

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include <google/protobuf/text_format.h>

namespace pickup {
namespace config {

/**
 * @brief 基于 protobuf 文本格式的配置基类模板
 *
 * @tparam ConfigType protobuf 消息类型
 */
template <typename ConfigType>
class ProtoBufConfig {
 public:
  virtual ~ProtoBufConfig() = default;

  /**
   * @brief 获取当前配置
   * @return 配置的常量引用
   */
  const ConfigType& config() const;

  /**
   * @brief 从文件加载配置（文本格式）
   * @param conf_file 配置文件路径
   * @return 成功返回 true，文件打开失败或解析失败返回 false
   */
  bool loadFromFile(const std::string& conf_file);

  /**
   * @brief 从字符串加载配置（文本格式）
   * @param content 文本格式的配置内容
   * @return 成功返回 true，解析失败返回 false
   */
  bool loadFromString(const std::string& content);

  /**
   * @brief 将当前配置写入文件（文本格式）
   * @param dump_file 目标文件路径
   * @return 成功返回 true，写入失败返回 false
   */
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
