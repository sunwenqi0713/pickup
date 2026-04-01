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
 * @brief 检查字符串是否为空或全为空白字符（空格、\t、\n、\r 等）
 * @param str 要检查的字符串
 * @return 为空或全空白返回 true，否则返回 false
 */
bool isBlank(const std::string& str);

/**
 * @brief 检查字符串是否包含指定子串
 * @param str 要搜索的字符串
 * @param pattern 要查找的子串
 * @return 包含返回 true，否则返回 false
 */
bool contains(const std::string& str, const std::string& pattern);

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
 * @brief 移除字符串左侧属于指定字符集的字符
 * @param str 要处理的字符串（原地修改）
 * @param chars 要移除的字符集，如 " \t\n\r"
 */
void trimLeft(std::string& str, const std::string& chars);

/**
 * @brief 移除字符串右侧属于指定字符集的字符
 * @param str 要处理的字符串（原地修改）
 * @param chars 要移除的字符集，如 " \t\n\r"
 */
void trimRight(std::string& str, const std::string& chars);

/**
 * @brief 移除字符串两侧属于指定字符集的字符
 * @param str 要处理的字符串（原地修改）
 * @param chars 要移除的字符集，如 " \t\n\r"
 */
void trim(std::string& str, const std::string& chars);

/**
 * @brief 替换字符串中所有匹配的子串
 * @param str 原始字符串
 * @param from 要被替换的子串
 * @param to 替换后的子串
 * @return 替换后的新字符串，原字符串不变
 */
std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

/**
 * @brief 若字符串以指定前缀开头则将其移除，否则原样返回
 * @param str    原始字符串
 * @param prefix 要移除的前缀
 * @return 移除前缀后的新字符串
 */
std::string stripPrefix(const std::string& str, const std::string& prefix);

/**
 * @brief 若字符串以指定后缀结尾则将其移除，否则原样返回
 * @param str    原始字符串
 * @param suffix 要移除的后缀
 * @return 移除后缀后的新字符串
 */
std::string stripSuffix(const std::string& str, const std::string& suffix);

/**
 * @brief 在字符串左侧填充字符至指定宽度
 * @param str   原始字符串
 * @param width 目标宽度；str 已达到或超过此宽度时原样返回
 * @param fill  填充字符，默认为空格
 * @return 填充后的新字符串
 */
std::string padLeft(const std::string& str, size_t width, char fill = ' ');

/**
 * @brief 在字符串右侧填充字符至指定宽度
 * @param str   原始字符串
 * @param width 目标宽度；str 已达到或超过此宽度时原样返回
 * @param fill  填充字符，默认为空格
 * @return 填充后的新字符串
 */
std::string padRight(const std::string& str, size_t width, char fill = ' ');

/**
 * @brief 将字符串重复 n 次
 * @param str 要重复的字符串
 * @param n   重复次数；为 0 时返回空字符串
 * @return 重复后的新字符串
 */
std::string repeat(const std::string& str, size_t n);

/**
 * @brief 使用分隔符分割字符串
 * @param str    要分割的字符串
 * @param delims 分隔符字符集（每个字符各自独立作为分隔符）
 * @return 分割后的子串向量
 */
std::vector<std::string> split(const std::string& str, const std::string& delims);

/**
 * @brief 按完整字符串分隔符分割
 * @details 与 split 的区别：delimiter 作为整体匹配，不是逐字符。
 *          例：splitBy("a::b::c", "::") → ["a","b","c"]，
 *          而  split("a::b::c", "::") → ["a","","b","","c"]
 * @param str       要分割的字符串
 * @param delimiter 分隔字符串，如 "::" 或 ", "
 * @return 分割后的子串向量；delimiter 为空时返回逐字符向量
 */
std::vector<std::string> splitBy(const std::string& str, const std::string& delimiter);

/**
 * @brief 使用字符分隔符连接字符串向量
 * @param glue   连接用的分隔字符
 * @param pieces 要连接的字符串向量
 * @return 连接后的字符串
 */
std::string join(char glue, const std::vector<std::string>& pieces);

/**
 * @brief 使用字符串分隔符连接字符串向量
 * @param glue   连接用的分隔字符串
 * @param pieces 要连接的字符串向量
 * @return 连接后的字符串
 */
std::string join(const std::string& glue, const std::vector<std::string>& pieces);

}  // namespace utils
}  // namespace pickup
