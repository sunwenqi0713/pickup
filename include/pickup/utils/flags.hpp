#pragma once

#include <bitset>
#include <type_traits>

namespace pickup {
namespace utils {

/** @brief Enum-based flags false checker */
template <typename TEnum>
struct IsEnumFlags : public std::false_type {};

/** @brief Register a new enum-based flags macro
    @details Should be used to get access to AND/OR/XOR logical operators with a given enum values.

    Example:
    @code{.cpp}
    enum class MyFlags
    {
        None  = 0x0,
        One   = 0x1,
        Two   = 0x2,
        Three = 0x4,
        Four  = 0x8
    };

    ENUM_FLAGS(MyFlags)

    int main()
    {
        auto mask = MyFlags::One | MyFlags::Two;
        if (mask & MyFlags::Two)
        {
            ...
        }
    }
    @endcode
*/
#define ENUM_FLAGS(type)                        \
  using pickup::utils::operator&;               \
  using pickup::utils::operator|;               \
  using pickup::utils::operator^;               \
  namespace pickup::utils {                     \
  template <>                                   \
  struct IsEnumFlags<type> : std::true_type {}; \
  }

/** @brief Enum-based flags
    @details Helper class for enum based flags which wraps particular enum as a template parameter
    and provides flags manipulation operators and methods.
    @note Not thread-safe.
*/
template <typename TEnum>
class Flags {
  /** @brief Enum underlying type */
  typedef typename std::make_unsigned<typename std::underlying_type<TEnum>::type>::type type;

 public:
  Flags() noexcept : value_(0) {}
  Flags(type value) noexcept : value_(value) {}
  Flags(TEnum value) noexcept : value_((type)value) {}
  Flags(const Flags&) noexcept = default;
  Flags(Flags&&) noexcept = default;
  ~Flags() noexcept = default;

  Flags& operator=(type value) noexcept {
    value_ = value;
    return *this;
  }
  Flags& operator=(TEnum value) noexcept {
    value_ = (type)value;
    return *this;
  }
  Flags& operator=(const Flags&) noexcept = default;
  Flags& operator=(Flags&&) noexcept = default;

  /** @brief Is any flag set? */
  explicit operator bool() const noexcept { return isset(); }

  /** @brief Is no flag set? */
  bool operator!() const noexcept { return !isset(); }

  /** @brief Reverse all flags */
  Flags operator~() const noexcept { return Flags(~value_); }

  /** @brief Flags logical assign operators */
  Flags& operator&=(const Flags& flags) noexcept {
    value_ &= flags.value_;
    return *this;
  }
  Flags& operator|=(const Flags& flags) noexcept {
    value_ |= flags.value_;
    return *this;
  }
  Flags& operator^=(const Flags& flags) noexcept {
    value_ ^= flags.value_;
    return *this;
  }

  /** @brief Flags logical friend operators */
  friend Flags operator&(const Flags& flags1, const Flags& flags2) noexcept {
    return Flags(flags1.value_ & flags2.value_);
  }
  friend Flags operator|(const Flags& flags1, const Flags& flags2) noexcept {
    return Flags(flags1.value_ | flags2.value_);
  }
  friend Flags operator^(const Flags& flags1, const Flags& flags2) noexcept {
    return Flags(flags1.value_ ^ flags2.value_);
  }

  /** @brief Flags comparison */
  friend bool operator==(const Flags& flags1, const Flags& flags2) noexcept { return flags1.value_ == flags2.value_; }
  friend bool operator!=(const Flags& flags1, const Flags& flags2) noexcept { return flags1.value_ != flags2.value_; }

  /** @brief Convert to the enum value */
  operator TEnum() const noexcept { return (TEnum)value_; }

  /** @brief Is any flag set? */
  bool isset() const noexcept { return (value_ != 0); }
  /** @brief Is the given flag set? */
  bool isset(type value) const noexcept { return (value_ & value) != 0; }
  /** @brief Is the given flag set? */
  bool isset(TEnum value) const noexcept { return (value_ & (type)value) != 0; }

  /** @brief Get the enum value */
  TEnum value() const noexcept { return (TEnum)value_; }
  /** @brief Get the underlying enum value */
  type underlying() const noexcept { return value_; }
  /** @brief Get the bitset value */
  std::bitset<sizeof(type) * 8> bitset() const noexcept { return {value_}; }

  /** @brief Swap two instances */
  void swap(Flags& flags) noexcept {
    using std::swap;
    swap(value_, flags.value_);
  }
  template <typename UEnum>
  friend void swap(Flags<UEnum>& flags1, Flags<UEnum>& flags2) noexcept;

 private:
  type value_;
};

/** @brief Swap two flags */
template <typename TEnum>
inline void swap(Flags<TEnum>& flags1, Flags<TEnum>& flags2) noexcept {
  flags1.swap(flags2);
}

/** @brief Logical operators for enum-based flags */
template <typename TEnum>
constexpr auto operator&(TEnum value1, TEnum value2) noexcept ->
    typename std::enable_if<IsEnumFlags<TEnum>::value, Flags<TEnum>>::type {
  return Flags<TEnum>(value1) & value2;
}

template <typename TEnum>
constexpr auto operator|(TEnum value1, TEnum value2) noexcept ->
    typename std::enable_if<IsEnumFlags<TEnum>::value, Flags<TEnum>>::type {
  return Flags<TEnum>(value1) | value2;
}

template <typename TEnum>
constexpr auto operator^(TEnum value1, TEnum value2) noexcept ->
    typename std::enable_if<IsEnumFlags<TEnum>::value, Flags<TEnum>>::type {
  return Flags<TEnum>(value1) ^ value2;
}

}  // namespace utils
}  // namespace pickup
