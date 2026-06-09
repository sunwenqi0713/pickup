#include "pickup/thread/Thread.h"

#include <cstring>
#include <string>
#include <thread>
#include <utility>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#include <pthread.h>
#include <pthread_np.h>
#elif defined(__linux__)
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <pthread.h>
#endif

namespace pickup {
namespace thread {

// === this_thread ===

namespace this_thread {

long getThreadId() {
#if defined(__linux__) || defined(__ANDROID__)
  return static_cast<long>(::syscall(SYS_gettid));
#elif defined(__APPLE__)
  uint64_t tid;
  pthread_threadid_np(NULL, &tid);
  return static_cast<long>(tid);
#elif defined(_WIN32)
  return static_cast<long>(::GetCurrentThreadId());
#else
  std::thread::id this_id = std::this_thread::get_id();
  return static_cast<long>(std::hash<std::thread::id>{}(this_id));
#endif
}

void setName(const std::string& name) {
#if defined(_WIN32)
  std::wstring wname(name.begin(), name.end());
  ::SetThreadDescription(::GetCurrentThread(), wname.c_str());
#elif defined(PR_SET_NAME)
  ::prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
  pthread_set_name_np(pthread_self(), name.c_str());
#elif defined(__APPLE__)
  pthread_setname_np(name.c_str());
#else
  (void)name;
#endif
}

std::string getName() {
#if defined(_WIN32)
  PWSTR wname = nullptr;
  if (SUCCEEDED(::GetThreadDescription(::GetCurrentThread(), &wname))) {
    int len = ::WideCharToMultiByte(CP_UTF8, 0, wname, -1, nullptr, 0, nullptr, nullptr);
    std::string result(len - 1, '\0');
    ::WideCharToMultiByte(CP_UTF8, 0, wname, -1, result.data(), len, nullptr, nullptr);
    ::LocalFree(wname);
    return result;
  }
  return std::string();
#elif defined(PR_GET_NAME)
  char name[16] = {};
  ::prctl(PR_GET_NAME, name, 0, 0, 0);
  return std::string(name);
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
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

}  // namespace this_thread

// === Thread ===

Thread::~Thread() {
  if (thread_.joinable()) {
    thread_.detach();
  }
}

std::thread::id Thread::getId() const noexcept {
  return thread_.get_id();
}

bool Thread::joinable() const noexcept {
  return thread_.joinable();
}

void Thread::join() {
  thread_.join();
}

void Thread::detach() {
  thread_.detach();
}

void Thread::swap(Thread& other) noexcept {
  using std::swap;
  swap(name_, other.name_);
  swap(thread_, other.thread_);
}

size_t Thread::hardwareConcurrency() noexcept {
  return std::thread::hardware_concurrency();
}

void Thread::startThread(std::function<void()> task,
                         std::function<void(const std::string&)> onStarted,
                         std::function<void(const std::string&)> onTerminated) {
  thread_ = std::thread(
      [this, task = std::move(task),
       onStarted = std::move(onStarted),
       onTerminated = std::move(onTerminated)]() {
        this_thread::setName(name_);

        if (onStarted) {
          onStarted(name_);
        }

        task();

        if (onTerminated) {
          onTerminated(name_);
        }
      });
}

}  // namespace thread
}  // namespace pickup
