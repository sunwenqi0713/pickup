#pragma once

#include <string>
#include <vector>

namespace pickup {
namespace utils {

/**
 * @brief 将字符串转换为大写
 * 
 * @param[in] str 输入字符串
 * @return 转换为大写后的新字符串，原字符串不变
 */
std::string toUpper(const std::string& str);

/**
 * @brief 将字符串转换为小写
 * 
 * @param[in] str 输入字符串
 * @return 转换为小写后的新字符串，原字符串不变
 */
std::string toLower(const std::string& str);

/**
 * @brief 检查字符串是否以指定字符开头
 * 
 * @param[in] str 要检查的字符串
 * @param[in] ch 要检查的字符
 * @return true 如果字符串以指定字符开头
 * @return false 如果字符串为空或不以指定字符开头
 */
bool startsWith(const std::string& str, char ch);

/**
 * @brief 检查字符串是否以指定前缀开头
 * 
 * @param[in] str 要检查的字符串
 * @param[in] prefix 要检查的前缀字符串
 * @return true 如果字符串以指定前缀开头
 * @return false 如果字符串为空或长度小于前缀长度
 */
bool startsWith(const std::string& str, const std::string& prefix);

/**
 * @brief 检查字符串是否以指定字符结尾
 * 
 * @param[in] str 要检查的字符串
 * @param[in] ch 要检查的字符
 * @return true 如果字符串以指定字符结尾
 * @return false 如果字符串为空或不以指定字符结尾
 */
bool endsWith(const std::string& str, char ch);

/**
 * @brief 检查字符串是否以指定后缀结尾
 * 
 * @param[in] str 要检查的字符串
 * @param[in] suffix 要检查的后缀字符串
 * @return true 如果字符串以指定后缀结尾
 * @return false 如果字符串为空或长度小于后缀长度
 */
bool endsWith(const std::string& str, const std::string& suffix);

/**
 * @brief 检查字符串是否包含指定的子串
 * 
 * @param[in] str 要搜索的字符串
 * @param[in] pattern 要查找的子串
 * @return true 如果字符串包含指定子串
 * @return false 如果字符串为空或未找到子串
 */
bool contains(const std::string& str, const std::string& pattern);

/**
 * @brief 比较两个字符是否相等（不区分大小写）
 * 
 * @param[in] c1 第一个字符
 * @param[in] c2 第二个字符
 * @return true 如果两个字符相同（忽略大小写）
 * @return false 如果两个字符不同
 * 
 * @note 仅支持ASCII字符，对于非ASCII字符行为未定义
 */
bool compareNoCase(char c1, char c2);

/**
 * @brief 比较两个字符串是否相等（不区分大小写）
 * 
 * @param[in] str1 第一个字符串
 * @param[in] str2 第二个字符串
 * @return true 如果两个字符串长度相等且对应字符相同（忽略大小写）
 * @return false 如果字符串长度不同或存在不同字符
 * 
 * @note 仅支持ASCII字符，对于非ASCII字符行为未定义
 */
bool compareNoCase(const std::string& str1, const std::string& str2);

/**
 * @brief 移除字符串左侧的指定字符
 * 
 * @param[in,out] str 要处理的字符串（原地修改）
 * @param[in] c 要移除的字符，默认为空格
 * 
 * @note 如果字符串为空，函数不做任何操作
 */
void trimLeft(std::string& str, char c = ' ');

/**
 * @brief 移除字符串右侧的指定字符
 * 
 * @param[in,out] str 要处理的字符串（原地修改）
 * @param[in] c 要移除的字符，默认为空格
 * 
 * @note 如果字符串为空，函数不做任何操作
 */
void trimRight(std::string& str, char c = ' ');

/**
 * @brief 移除字符串两侧的指定字符
 * 
 * @param[in,out] str 要处理的字符串（原地修改）
 * @param[in] c 要移除的字符，默认为空格
 * 
 * @note 等价于先调用trimLeft再调用trimRight
 */
void trim(std::string& str, char c = ' ');

/**
 * @brief 替换字符串中所有匹配的子串
 * 
 * @param[in] str 原始字符串
 * @param[in] from 要被替换的子串
 * @param[in] to 替换后的子串
 * @return 替换后的新字符串，原字符串不变
 * 
 * @note 如果from为空字符串，函数返回原始字符串
 */
std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

/**
 * @brief 使用分隔符分割字符串
 * 
 * @param[in] str 要分割的字符串
 * @param[in] delims 分隔符字符串，每个字符都视为独立的分隔符
 * @return 分割后的子串向量，不包含空字符串
 * 
 * @example
 * split("a,b;c", ",;") 返回 {"a", "b", "c"}
 * split("a,,b", ",") 返回 {"a", "b"}
 */
std::vector<std::string> split(const std::string& str, const std::string& delims);

/**
 * @brief 使用字符分隔符连接字符串向量
 * 
 * @param[in] glue 连接用的分隔字符
 * @param[in] pieces 要连接的字符串向量
 * @return 连接后的字符串
 * 
 * @example
 * join(',', {"a", "b", "c"}) 返回 "a,b,c"
 * 如果pieces为空，返回空字符串
 */
std::string join(char glue, const std::vector<std::string>& pieces);

/**
 * @brief 使用字符串分隔符连接字符串向量
 * 
 * @param[in] glue 连接用的分隔字符串
 * @param[in] pieces 要连接的字符串向量
 * @return 连接后的字符串
 * 
 * @example
 * join("->", {"a", "b", "c"}) 返回 "a->b->c"
 * 如果pieces为空，返回空字符串
 */
std::string join(const std::string& glue, const std::vector<std::string>& pieces);

}  // namespace utils
}  // namespace pickup