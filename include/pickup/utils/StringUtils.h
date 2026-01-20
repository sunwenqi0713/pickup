#pragma once

#include <string>
#include <vector>

namespace pickup {
namespace utils {

/**
 * @brief 将字符串转换为大写
 * @param str 输入字符串
 * @return 转换后的新字符串，原字符串不变
 */
std::string toUpper(const std::string& str);

/**
 * @brief 将字符串转换为小写
 * @param str 输入字符串
 * @return 转换后的新字符串，原字符串不变
 */
std::string toLower(const std::string& str);

/**
 * @brief 检查字符串是否以指定字符开头
 * @param str 要检查的字符串
 * @param ch 要检查的字符
 * @return 匹配返回 true，否则返回 false
 */
bool startsWith(const std::string& str, char ch);

/**
 * @brief 检查字符串是否以指定前缀开头
 * @param str 要检查的字符串
 * @param prefix 要检查的前缀
 * @return 匹配返回 true，否则返回 false
 */
bool startsWith(const std::string& str, const std::string& prefix);

/**
 * @brief 检查字符串是否以指定字符结尾
 * @param str 要检查的字符串
 * @param ch 要检查的字符
 * @return 匹配返回 true，否则返回 false
 */
bool endsWith(const std::string& str, char ch);

/**
 * @brief 检查字符串是否以指定后缀结尾
 * @param str 要检查的字符串
 * @param suffix 要检查的后缀
 * @return 匹配返回 true，否则返回 false
 */
bool endsWith(const std::string& str, const std::string& suffix);

/**
 * @brief 检查字符串是否包含指定子串
 * @param str 要搜索的字符串
 * @param pattern 要查找的子串
 * @return 包含返回 true，否则返回 false
 */
bool contains(const std::string& str, const std::string& pattern);

/**
 * @brief 比较两个字符是否相等（不区分大小写）
 * @param c1 第一个字符
 * @param c2 第二个字符
 * @return 相等返回 true，否则返回 false
 * @note 仅支持 ASCII 字符
 */
bool compareNoCase(char c1, char c2);

/**
 * @brief 比较两个字符串是否相等（不区分大小写）
 * @param str1 第一个字符串
 * @param str2 第二个字符串
 * @return 相等返回 true，否则返回 false
 * @note 仅支持 ASCII 字符
 */
bool compareNoCase(const std::string& str1, const std::string& str2);

/**
 * @brief 移除字符串左侧的指定字符
 * @param str 要处理的字符串（原地修改）
 * @param c 要移除的字符，默认为空格
 */
void trimLeft(std::string& str, char c = ' ');

/**
 * @brief 移除字符串右侧的指定字符
 * @param str 要处理的字符串（原地修改）
 * @param c 要移除的字符，默认为空格
 */
void trimRight(std::string& str, char c = ' ');

/**
 * @brief 移除字符串两侧的指定字符
 * @param str 要处理的字符串（原地修改）
 * @param c 要移除的字符，默认为空格
 */
void trim(std::string& str, char c = ' ');

/**
 * @brief 替换字符串中所有匹配的子串
 * @param str 原始字符串
 * @param from 要被替换的子串
 * @param to 替换后的子串
 * @return 替换后的新字符串，原字符串不变
 */
std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

/**
 * @brief 使用分隔符分割字符串
 * @param str 要分割的字符串
 * @param delims 分隔符（每个字符都视为独立分隔符）
 * @return 分割后的子串向量
 */
std::vector<std::string> split(const std::string& str, const std::string& delims);

/**
 * @brief 使用字符分隔符连接字符串向量
 * @param glue 连接用的分隔字符
 * @param pieces 要连接的字符串向量
 * @return 连接后的字符串
 */
std::string join(char glue, const std::vector<std::string>& pieces);

/**
 * @brief 使用字符串分隔符连接字符串向量
 * @param glue 连接用的分隔字符串
 * @param pieces 要连接的字符串向量
 * @return 连接后的字符串
 */
std::string join(const std::string& glue, const std::vector<std::string>& pieces);

}  // namespace utils
}  // namespace pickup