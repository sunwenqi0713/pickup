#pragma once

#include <string>
#include <vector>

namespace pickup {
namespace utils {
// 判断字符串 s 是否以 ch 开头
bool startsWith(const std::string& s, char ch);
// 判断字符串 s 是否以 prefix 开头
bool startsWith(const std::string& s, const std::string& prefix);
// 判断字符串 s 是否以 ch 结尾
bool endsWith(const std::string& s, char ch);
// 判断字符串 s 是否以 suffix 结尾
bool endsWith(const std::string& s, const std::string& suffix);

// 判断字符串 s 是否包含 parts
bool contains(const std::string& s, const std::string& parts);
// 使用 delimiters 分割字符串 s
std::vector<std::string> split(const std::string& s, const std::string& delimiters);

// 将字符串 s 转换为大写
std::string toUpper(std::string s);
// 将字符串 s 转换为小写
std::string toLower(std::string s);

// 去除字符串 s 左侧的空白字符
std::string& trimLeft(std::string& s, const std::string& whitespace = " ");
// 去除字符串 s 右侧的空白字符
std::string& trimRight(std::string& s, const std::string& whitespace = " ");
// 去除字符串 s 两侧的空白字符
std::string& trim(std::string& s, const std::string& whitespace = " ");

// 替换字符串 s 中的 from 为 to
size_t replace(std::string& s, const std::string& from, const std::string& to);

// 使用 glue 连接字符串 pieces
std::string join(char glue, const std::vector<std::string>& pieces);
// 重载版本：使用字符串 glue 连接字符串 pieces
std::string join(const std::string& glue, const std::vector<std::string>& pieces);

}  // namespace utils
}  // namespace pickup