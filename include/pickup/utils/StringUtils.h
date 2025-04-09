#pragma once

#include <string>
#include <vector>

namespace pickup {
namespace utils {
// 判断字符串 str 是否以 ch 开头
bool startsWith(const std::string& str, char ch);
// 判断字符串 str 是否以 prefix 开头
bool startsWith(const std::string& str, const std::string& prefix);
// 判断字符串 str 是否以 ch 结尾
bool endsWith(const std::string& str, char ch);
// 判断字符串 str 是否以 suffix 结尾
bool endsWith(const std::string& str, const std::string& suffix);

// 判断字符串 str 是否包含 parts
bool contains(const std::string& str, const std::string& parts);

// 将字符串 str 转换为大写
std::string toUpper(std::string str);
// 将字符串 str 转换为小写
std::string toLower(std::string str);

// 比较两个字符串是否相等
bool compare(const std::string& str1, const std::string& str2);
bool compareNoCase(const std::string& str1, const std::string& str2);

// 去除字符串 str 左侧的空白字符
std::string& trimLeft(std::string& str, const std::string& whitespace = " ");
// 去除字符串 str 右侧的空白字符
std::string& trimRight(std::string& str, const std::string& whitespace = " ");
// 去除字符串 str 两侧的空白字符
std::string& trim(std::string& str, const std::string& whitespace = " ");

// 替换字符串 str 中的 from 为 to
bool replaceFirst(std::string& str, const std::string& from, const std::string& to);
bool replaceLast(std::string& str, const std::string& from, const std::string& to);
size_t replaceAll(std::string& str, const std::string& from, const std::string& to);

// 使用 delimiters 分割字符串 str
std::vector<std::string> split(const std::string& str, const std::string& delimiters);

// 使用 glue 连接字符串 pieces
std::string join(char glue, const std::vector<std::string>& pieces);
// 重载版本：使用字符串 glue 连接字符串 pieces
std::string join(const std::string& glue, const std::vector<std::string>& pieces);

}  // namespace utils
}  // namespace pickup