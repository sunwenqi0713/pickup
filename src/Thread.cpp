#include "pickup/thread/Thread.h"

#include <cstring>
#include <string>
#include <thread>
#include <utility>

#if (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__))
#include <pthread.h>
#include <pthread_np.h>
#endif

#if __has_include(<sys/prctl.h>)
#include <sys/prctl.h>
#endif

namespace pickup {
namespace utils {

long getThreadID() {
#if defined(__linux__) || defined(__ANDROID__)
  return static_cast<long>(::syscall(SYS_gettid));
#elif defined(__APPLE__)
  uint64_t tid;
  pthread_threadid_np(NULL, &tid);
  return static_cast<long>(tid);
#else
  // Fallback to std::thread::id hash
  std::thread::id this_id = std::this_thread::get_id();
  return static_cast<long>(std::hash<std::thread::id>{}(this_id));
#endif
}

void setThreadName(const std::string& name) {
#if defined(PR_SET_NAME)
  // 仅使用前15个字符（16 - NUL终止符）
  // Linux系统使用prctl系统调用
  ::prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#elif (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__))
  // BSD系列系统使用pthread_set_name_np
  pthread_set_name_np(pthread_self(), name.c_str());
#elif defined(__APPLE__)
  // macOS系统使用pthread_setname_np
  pthread_setname_np(name.c_str());
#else
  // 不支持的平台，静默忽略以避免未使用参数警告
  (void)name;
#endif
}

std::string getThreadName() {
#if defined(PR_GET_NAME)
  char name[16] = {};
  ::prctl(PR_GET_NAME, name, 0, 0, 0);
  return std::string(name);
#elif (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__))
  char name[16] = {};
  pthread_get_name_np(pthread_self(), name, sizeof(name));
  return std::string(name);
#elif defined(__APPLE__)
  char name[64] = {};
  pthread_getname_np(pthread_self(), name, sizeof(name));
  return std::string(name);
#else
  return std::string();
#endif
}

}  // namespace utils
}  // namespace pickup
