#include "pickup/codec/base64.h"

#include <string>

#include "pickup/utils/StringUtils.h"

namespace pickup {
namespace codec {

// Base64 编码字符表
static const std::string kBase64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/**
 * @brief 判断一个字符是否是有效的base64字符
 * @param c 待测试的字符
 * @return 如果c是有效的base64字符则返回true
 */
static bool isBase64Char(unsigned char c) {
  return (c == 43 ||               // +
          (c >= 47 && c <= 57) ||  // /-9
          (c >= 65 && c <= 90) ||  // A-Z
          (c >= 97 && c <= 122));  // a-z
}

std::string base64Encode(unsigned char const* input, size_t len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char charArray3[3];  // 存储3个原始字节
  unsigned char charArray4[4];  // 存储4个base64编码后的字节

  while (len--) {
    charArray3[i++] = *(input++);  // 读取一个字节
    if (i == 3) {                  // 每攒够3个字节进行一次编码
      // 编码过程：将3个8位字节转换为4个6位base64字符
      charArray4[0] = (charArray3[0] & 0xfc) >> 2;
      charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
      charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
      charArray4[3] = charArray3[2] & 0x3f;

      // 将6位值映射到base64字符表
      for (i = 0; i < 4; i++) {
        ret += kBase64Chars[charArray4[i]];
      }
      i = 0;  // 重置计数器
    }
  }

  // 处理剩余不足3字节的情况（填充）
  if (i) {
    for (j = i; j < 3; j++) {
      charArray3[j] = '\0';  // 用0填充不足的字节
    }

    // 对剩余字节进行编码
    charArray4[0] = (charArray3[0] & 0xfc) >> 2;
    charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
    charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
    charArray4[3] = charArray3[2] & 0x3f;

    // 添加编码后的字符
    for (j = 0; j < i + 1; j++) {
      ret += kBase64Chars[charArray4[j]];
    }

    // 添加填充字符'='
    while (i++ < 3) {
      ret += '=';
    }
  }

  return ret;
}

std::string base64Decode(std::string const& input) {
  size_t inLen = input.size();  // 输入字符串长度
  int i = 0;
  int j = 0;
  int index = 0;                                 // 输入字符串索引
  unsigned char charArray4[4], charArray3[3];   // 解码用缓冲区
  std::string ret;                               // 返回的解码结果

  // 遍历输入字符串（忽略填充字符'='和非base64字符）
  while (inLen-- && (input[index] != '=') && isBase64Char(input[index])) {
    charArray4[i++] = input[index];
    index++;
    if (i == 4) {  // 每攒够4个base64字符进行一次解码
      // 将base64字符转换回6位值
      for (i = 0; i < 4; i++) {
        charArray4[i] = static_cast<unsigned char>(kBase64Chars.find(charArray4[i]));
      }

      // 解码过程：将4个6位base64字符转换为3个8位原始字节
      charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
      charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
      charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

      // 添加解码后的原始字节
      for (i = 0; i < 3; i++) {
        ret += charArray3[i];
      }
      i = 0;  // 重置计数器
    }
  }

  // 处理剩余的base64字符（可能包含填充）
  if (i) {
    // 用0填充不足的base64字符
    for (j = i; j < 4; j++) {
      charArray4[j] = 0;
    }

    // 将剩余base64字符转换回6位值
    for (j = 0; j < 4; j++) {
      charArray4[j] = static_cast<unsigned char>(kBase64Chars.find(charArray4[j]));
    }

    // 解码剩余字符
    charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
    charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
    charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

    // 添加有效解码字节（忽略填充产生的0字节）
    for (j = 0; j < i - 1; j++) {
      ret += static_cast<std::string::value_type>(charArray3[j]);
    }
  }

  return ret;
}

std::string base64UrlToBase64(const std::string& base64url) {
  std::string temp;
  temp.reserve(base64url.size() + 4);

  // change Base64 alphabet from urlsafe version to standard
  for (const auto& c : base64url) {
    if (c == '-') {
      temp += '+';
    } else if (c == '_') {
      temp += '/';
    } else {
      temp += c;
    }
  }

  // add padding
  if ((base64url.size() % 4) != 0u) {
    int toAppend = 4 - static_cast<int>(base64url.size() % 4);
    for (int i = 0; i < toAppend; i++) {
      temp += '=';
    }
  }

  return temp;
}

std::string base64ToBase64Url(const std::string& base64) {
  std::string temp(base64);

  // remove padding
  size_t found = temp.find_last_not_of('=');
  if (found == std::string::npos) return "";

  // change Base64 alphabet from standard version to urlsafe
  utils::replaceAll(temp, "+", "-");
  utils::replaceAll(temp, "/", "_");

  return temp.substr(0, found + 1);
}

}  // namespace codec
}  // namespace pickup
