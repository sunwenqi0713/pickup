#pragma once

#include <map>
#include <optional>
#include <set>
#include <string>

namespace pickup {
namespace config {

class INIReader {
 public:
  struct ParseError {
    enum class Kind { FileOpen, Syntax };
    Kind kind;
    int line = 0;
  };

  INIReader() = default;
  explicit INIReader(const std::string& filename);
  explicit INIReader(std::istream& stream);

  [[nodiscard]] std::optional<ParseError> error() const noexcept { return error_; }

  const std::set<std::string>& sections() const { return sections_; }

  bool hasSection(const std::string& section) const { return sections_.count(section) > 0; }

  bool hasValue(const std::string& section, const std::string& name) const;

  std::string get(const std::string& section, const std::string& name,
                  const std::string& default_value) const;

  long getInteger(const std::string& section, const std::string& name,
                  long default_value) const;

  double getReal(const std::string& section, const std::string& name,
                 double default_value) const;

  float getFloat(const std::string& section, const std::string& name,
                 float default_value) const;

  bool getBoolean(const std::string& section, const std::string& name,
                  bool default_value) const;

 private:
  std::optional<ParseError> error_;
  std::map<std::string, std::string> values_;
  std::set<std::string> sections_;

  static std::string makeKey(const std::string& section, const std::string& name);
  static std::string trimRight(const std::string& s);
  static std::string trimLeft(const std::string& s);
  static size_t findCharsOrComment(const std::string& s, const std::string& chars);
  void parse(std::istream& stream);
};

}  // namespace config
}  // namespace pickup
