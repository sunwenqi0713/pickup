#include "pickup/codec/base64.h"

#include <string>

namespace pickup {
namespace codec {

// Base64 编码字符表
static std::string const base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/// 判断一个字符是否是有效的base64字符
/**
 * @param c 待测试的字符
 * @return 如果c是有效的base64字符则返回true
 */
static inline bool is_base64(unsigned char c) {
  return (c == 43 ||               // +
          (c >= 47 && c <= 57) ||  // /-9
          (c >= 65 && c <= 90) ||  // A-Z
          (c >= 97 && c <= 122));  // a-z
}

inline std::string base64_encode(unsigned char const* input, size_t len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];  // 存储3个原始字节
  unsigned char char_array_4[4];  // 存储4个base64编码后的字节

  while (len--) {
    char_array_3[i++] = *(input++);  // 读取一个字节
    if (i == 3) {                    // 每攒够3个字节进行一次编码
      // 编码过程：将3个8位字节转换为4个6位base64字符
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      // 将6位值映射到base64字符表
      for (i = 0; (i < 4); i++) {
        ret += base64_chars[char_array_4[i]];
      }
      i = 0;  // 重置计数器
    }
  }

  // 处理剩余不足3字节的情况（填充）
  if (i) {
    for (j = i; j < 3; j++) {
      char_array_3[j] = '\0';  // 用0填充不足的字节
    }

    // 对剩余字节进行编码
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    // 添加编码后的字符
    for (j = 0; (j < i + 1); j++) {
      ret += base64_chars[char_array_4[j]];
    }

    // 添加填充字符'='
    while ((i++ < 3)) {
      ret += '=';
    }
  }

  return ret;
}

inline std::string base64_decode(std::string const& input) {
  size_t in_len = input.size();  // 输入字符串长度
  int i = 0;
  int j = 0;
  int index = 0;                                     // 输入字符串索引
  unsigned char char_array_4[4], char_array_3[3];  // 解码用缓冲区
  std::string ret;                                 // 返回的解码结果

  // 遍历输入字符串（忽略填充字符'='和非base64字符）
  while (in_len-- && (input[index] != '=') && is_base64(input[index])) {
    char_array_4[i++] = input[index];
    index++;
    if (i == 4) {  // 每攒够4个base64字符进行一次解码
      // 将base64字符转换回6位值
      for (i = 0; i < 4; i++) {
        char_array_4[i] = static_cast<unsigned char>(base64_chars.find(char_array_4[i]));
      }

      // 解码过程：将4个6位base64字符转换为3个8位原始字节
      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      // 添加解码后的原始字节
      for (i = 0; (i < 3); i++) {
        ret += char_array_3[i];
      }
      i = 0;  // 重置计数器
    }
  }

  // 处理剩余的base64字符（可能包含填充）
  if (i) {
    // 用0填充不足的base64字符
    for (j = i; j < 4; j++) char_array_4[j] = 0;

    // 将剩余base64字符转换回6位值
    for (j = 0; j < 4; j++) char_array_4[j] = static_cast<unsigned char>(base64_chars.find(char_array_4[j]));

    // 解码剩余字符
    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    // 添加有效解码字节（忽略填充产生的0字节）
    for (j = 0; (j < i - 1); j++) {
      ret += static_cast<std::string::value_type>(char_array_3[j]);
    }
  }

  return ret;
}

}  // namespace codec
}  // namespace pickup
