#include "pickup/config/INIReader.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

namespace pickup {
namespace config {

INIReader::INIReader(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open())
    error_ = ParseError{ParseError::Kind::FileOpen};
  else
    parse(file);
}

INIReader::INIReader(std::istream& stream) {
  parse(stream);
}

bool INIReader::hasValue(const std::string& section, const std::string& name) const {
  return values_.count(makeKey(section, name)) > 0;
}

std::string INIReader::get(const std::string& section, const std::string& name,
                           const std::string& default_value) const {
  auto it = values_.find(makeKey(section, name));
  return it != values_.end() ? it->second : default_value;
}

long INIReader::getInteger(const std::string& section, const std::string& name,
                           long default_value) const {
  std::string valstr = get(section, name, "");
  if (valstr.empty()) return default_value;
  try {
    size_t idx;
    long n = std::stol(valstr, &idx, 0);
    return idx > 0 ? n : default_value;
  } catch (...) {
    return default_value;
  }
}

double INIReader::getReal(const std::string& section, const std::string& name,
                          double default_value) const {
  std::string valstr = get(section, name, "");
  if (valstr.empty()) return default_value;
  try {
    size_t idx;
    double n = std::stod(valstr, &idx);
    return idx > 0 ? n : default_value;
  } catch (...) {
    return default_value;
  }
}

float INIReader::getFloat(const std::string& section, const std::string& name,
                          float default_value) const {
  std::string valstr = get(section, name, "");
  if (valstr.empty()) return default_value;
  try {
    size_t idx;
    float n = std::stof(valstr, &idx);
    return idx > 0 ? n : default_value;
  } catch (...) {
    return default_value;
  }
}

bool INIReader::getBoolean(const std::string& section, const std::string& name,
                           bool default_value) const {
  std::string valstr = get(section, name, "");
  std::transform(valstr.begin(), valstr.end(), valstr.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
    return true;
  if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
    return false;
  return default_value;
}

std::string INIReader::makeKey(const std::string& section, const std::string& name) {
  std::string key = section + "=" + name;
  std::transform(key.begin(), key.end(), key.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return key;
}

std::string INIReader::trimRight(const std::string& s) {
  auto end = s.end();
  while (end != s.begin() && std::isspace(static_cast<unsigned char>(*(end - 1))))
    --end;
  return {s.begin(), end};
}

std::string INIReader::trimLeft(const std::string& s) {
  auto begin = s.begin();
  while (begin != s.end() && std::isspace(static_cast<unsigned char>(*begin)))
    ++begin;
  return {begin, s.end()};
}

size_t INIReader::findCharsOrComment(const std::string& s, const std::string& chars) {
  bool was_space = false;
  for (size_t i = 0; i < s.size(); ++i) {
    char c = s[i];
    if (!chars.empty() && chars.find(c) != std::string::npos)
      return i;
    if (was_space && c == ';')
      return i;
    was_space = std::isspace(static_cast<unsigned char>(c)) != 0;
  }
  return s.size();
}

void INIReader::parse(std::istream& stream) {
  std::string section;
  std::string prev_name;
  int lineno = 0;

  for (std::string raw_line; std::getline(stream, raw_line);) {
    ++lineno;

    if (!raw_line.empty() && raw_line.back() == '\r')
      raw_line.pop_back();

    if (lineno == 1 && raw_line.size() >= 3 &&
        static_cast<unsigned char>(raw_line[0]) == 0xEF &&
        static_cast<unsigned char>(raw_line[1]) == 0xBB &&
        static_cast<unsigned char>(raw_line[2]) == 0xBF) {
      raw_line = raw_line.substr(3);
    }

    bool has_leading_space =
        !raw_line.empty() && std::isspace(static_cast<unsigned char>(raw_line[0]));
    std::string start = trimLeft(trimRight(raw_line));

    if (start.empty() || start[0] == ';' || start[0] == '#') {
    } else if (!prev_name.empty() && has_leading_space) {
      size_t end = findCharsOrComment(start, "");
      std::string val = trimRight(start.substr(0, end));
      values_[makeKey(section, prev_name)] += "\n" + val;
    } else if (start[0] == '[') {
      std::string after = start.substr(1);
      size_t end = findCharsOrComment(after, "]");
      if (end < after.size() && after[end] == ']') {
        section = after.substr(0, end);
        prev_name.clear();
      } else if (!error_) {
        error_ = ParseError{ParseError::Kind::Syntax, lineno};
      }
    } else {
      size_t sep = findCharsOrComment(start, "=:");
      if (sep < start.size() && (start[sep] == '=' || start[sep] == ':')) {
        std::string name = trimRight(start.substr(0, sep));
        std::string value = trimLeft(start.substr(sep + 1));
        size_t vend = findCharsOrComment(value, "");
        value = trimRight(value.substr(0, vend));

        prev_name = name;
        std::string& stored = values_[makeKey(section, name)];
        if (!stored.empty()) stored += "\n";
        stored += value;
        sections_.insert(section);
      } else if (!error_) {
        error_ = ParseError{ParseError::Kind::Syntax, lineno};
      }
    }
  }
}

}  // namespace config
}  // namespace pickup
