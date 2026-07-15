#include "pickup/time/Timezone.h"

#include <cstdint>
#include <string>
#include <utility>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

namespace pickup {
namespace time {

namespace {

#if defined(_WIN32) || defined(_WIN64)
// 从系统读取本地时区名并转为 UTF-8（避免 strftime "%Z" 返回本地代码页编码）
std::string localZoneName() {
  TIME_ZONE_INFORMATION tzi;
  const DWORD res = GetTimeZoneInformation(&tzi);
  const wchar_t* wname = (res == TIME_ZONE_ID_DAYLIGHT) ? tzi.DaylightName : tzi.StandardName;
  if (wname == nullptr || wname[0] == L'\0') return {};

  const int len = WideCharToMultiByte(CP_UTF8, 0, wname, -1, nullptr, 0, nullptr, nullptr);
  if (len <= 1) return {};
  std::string out(static_cast<size_t>(len - 1), '\0');
  WideCharToMultiByte(CP_UTF8, 0, wname, -1, out.data(), len, nullptr, nullptr);
  return out;
}
#endif

}  // namespace

Timezone::Timezone() {
  // 以当前时刻计算本地时区总偏移（含夏令时），复用 Time 的换算逻辑，
  // 保持跨平台一致，避免平台相关的时区偏移 API。
  const Timestamp now(Timestamp::utc());
  const LocalTime lt(now);

  // 将本地挂钟字段按 UTC 解释即得到 now + 偏移，两者之差即为本地相对 UTC 的偏移
  const int64_t offsetNs = lt.utcstamp().total() - now.total();
  offset_ = Timespan(offsetNs);
  dstoffset_ = Timespan::zero();

#if defined(_WIN32) || defined(_WIN64)
  name_ = localZoneName();
#else
  // POSIX 下 "%Z" 返回 ASCII 缩写（如 CST、UTC），可直接使用
  name_ = lt.formatted("%Z");
#endif
}

void Timezone::swap(Timezone& other) noexcept {
  using std::swap;
  swap(name_, other.name_);
  offset_.swap(other.offset_);
  dstoffset_.swap(other.dstoffset_);
}

}  // namespace time
}  // namespace pickup