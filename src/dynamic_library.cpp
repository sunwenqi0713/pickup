#include "pickup/utils/dynamic_library.h"

#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace pickup {
namespace utils {

DynamicLibrary::~DynamicLibrary() { unload(); }

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
    : handle_(std::exchange(other.handle_, nullptr)),
      path_(std::move(other.path_)),
      last_error_(std::move(other.last_error_)) {}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept {
  if (this != &other) {
    unload();
    handle_ = std::exchange(other.handle_, nullptr);
    path_ = std::move(other.path_);
    last_error_ = std::move(other.last_error_);
  }
  return *this;
}

bool DynamicLibrary::load(const std::string& path) {
  unload();
  path_ = path;

#ifdef _WIN32
  handle_ = LoadLibraryA(path_.c_str());
  if (!handle_) {
    last_error_ = "LoadLibrary failed, error code: " + std::to_string(GetLastError());
    return false;
  }
#else
  dlerror();  // 清除上一次错误
  handle_ = dlopen(path_.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (!handle_) {
    const char* err = dlerror();
    last_error_ = err ? err : "unknown error";
    return false;
  }
#endif
  return true;
}

void DynamicLibrary::unload() {
  if (!handle_) return;

#ifdef _WIN32
  if (!FreeLibrary(static_cast<HMODULE>(handle_))) {
    last_error_ = "FreeLibrary failed, error code: " + std::to_string(GetLastError());
  }
#else
  dlerror();
  if (dlclose(handle_) != 0) {
    const char* err = dlerror();
    last_error_ = err ? err : "unknown error";
  }
#endif
  handle_ = nullptr;
  path_.clear();
}

DynamicLibrary::SymbolType DynamicLibrary::getSymbol(const std::string& name) {
  if (!handle_) {
    last_error_ = "library not loaded";
    return nullptr;
  }

#ifdef _WIN32
  auto symbol = reinterpret_cast<SymbolType>(
      GetProcAddress(static_cast<HMODULE>(handle_), name.c_str()));
  if (!symbol) {
    last_error_ = "symbol not found: " + name +
                  ", error code: " + std::to_string(GetLastError());
  }
  return symbol;
#else
  dlerror();  // 清除上一次错误，确保 dlerror() 反映本次调用结果
  auto symbol = dlsym(handle_, name.c_str());
  const char* err = dlerror();
  if (err) last_error_ = err;
  return symbol;
#endif
}

}  // namespace utils
}  // namespace pickup
