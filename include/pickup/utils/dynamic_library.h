#pragma once

#include <string>

namespace pickup {
namespace utils {

/**
 * @brief 跨平台动态库加载封装
 */
class DynamicLibrary {
 public:
  using SymbolType = void*;
  using HandleType = void*;

  DynamicLibrary() = default;
  ~DynamicLibrary();

  DynamicLibrary(const DynamicLibrary&) = delete;
  DynamicLibrary& operator=(const DynamicLibrary&) = delete;

  DynamicLibrary(DynamicLibrary&& other) noexcept;
  DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

  /** @brief 加载动态库，失败时可通过 lastError() 获取原因 */
  bool load(const std::string& path);

  /** @brief 卸载动态库 */
  void unload();

  bool isLoaded() const { return handle_ != nullptr; }

  /**
   * @brief 获取符号地址，失败返回 nullptr，错误信息见 lastError()
   */
  SymbolType getSymbol(const std::string& name);

  const std::string& path() const { return path_; }
  const std::string& lastError() const { return last_error_; }

 private:
  HandleType handle_ = nullptr;
  std::string path_;
  std::string last_error_;
};

}  // namespace utils
}  // namespace pickup
