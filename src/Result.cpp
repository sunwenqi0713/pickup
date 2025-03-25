#include "pickup/utils/Result.h"

namespace pickup {
namespace utils {

Result::Result() noexcept {}

Result::Result(const std::string& message) noexcept : errorMessage_(message) {}

Result::Result(const Result& other) : errorMessage_(other.errorMessage_) {}

Result& Result::operator=(const Result& other) {
  errorMessage_ = other.errorMessage_;
  return *this;
}

Result::Result(Result&& other) noexcept : errorMessage_(std::move(other.errorMessage_)) {}

Result& Result::operator=(Result&& other) noexcept {
  errorMessage_ = std::move(other.errorMessage_);
  return *this;
}

bool Result::operator==(const Result& other) const noexcept { return errorMessage_ == other.errorMessage_; }

bool Result::operator!=(const Result& other) const noexcept { return errorMessage_ != other.errorMessage_; }

Result Result::fail(const std::string& errorMessage) noexcept {
  return Result(errorMessage.empty() ? "Unknown Error" : errorMessage);
}

bool Result::isOk() const noexcept { return errorMessage_.empty(); }
bool Result::failed() const noexcept { return !isOk(); }
Result::operator bool() const noexcept { return isOk(); }
bool Result::operator!() const noexcept { return failed(); }
const std::string& Result::getErrorMessage() const noexcept { return errorMessage_; }

}  // namespace utils
}  // namespace pickup