#include "pickup/codec/base64.h"

#include <string>

#include "pickup/utils/StringUtils.h"

namespace pickup {
namespace codec {
namespace base64 {

// Base64 编码字符表
static const std::string kBase64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// 判断一个字符是否是有效的 base64 字符
static bool isBase64Char(unsigned char c) {
  return (c == '+' ||
          (c >= '/' && c <= '9') ||  // /-9
          (c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z'));
}

std::string encode(unsigned char const* input, size_t len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char charArray3[3];  // 存储 3 个原始字节
  unsigned char charArray4[4];  // 存储 4 个 Base64 编码后的字节

  while (len--) {
    charArray3[i++] = *(input++);  // 读取一个字节
    if (i == 3) {                  // 每攒够 3 个字节进行一次编码
      // 编码过程：将 3 个 8 位字节转换为 4 个 6 位 Base64 字符
      charArray4[0] = (charArray3[0] & 0xfc) >> 2;
      charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
      charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
      charArray4[3] = charArray3[2] & 0x3f;

      // 将 6 位值映射到 Base64 字符表
      for (i = 0; i < 4; i++) {
        ret += kBase64Chars[charArray4[i]];
      }
      i = 0;  // 重置计数器
    }
  }

  // 处理剩余不足 3 字节的情况（填充）
  if (i) {
    for (j = i; j < 3; j++) {
      charArray3[j] = '\0';  // 用 0 填充不足的字节
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

    // 添加填充字符 '='
    while (i++ < 3) {
      ret += '=';
    }
  }

  return ret;
}

std::string decode(std::string const& input) {
  size_t inLen = input.size();  // 输入字符串长度
  int i = 0;
  int j = 0;
  int index = 0;                                 // 输入字符串索引
  unsigned char charArray4[4], charArray3[3];   // 解码用缓冲区
  std::string ret;                               // 返回的解码结果

  // 遍历输入字符串（忽略填充 '=' 和非 Base64 字符）
  while (inLen-- && (input[index] != '=') && isBase64Char(input[index])) {
    charArray4[i++] = input[index];
    index++;
    if (i == 4) {  // 每攒够 4 个 Base64 字符进行一次解码
      // 将 Base64 字符转换回 6 位值
      for (i = 0; i < 4; i++) {
        charArray4[i] = static_cast<unsigned char>(kBase64Chars.find(charArray4[i]));
      }

      // 解码过程：将 4 个 6 位 Base64 字符转换为 3 个 8 位原始字节
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

  // 处理剩余 Base64 字符（可能包含填充）
  if (i) {
    // 用 0 填充不足的 Base64 字符
    for (j = i; j < 4; j++) {
      charArray4[j] = 0;
    }

    // 将剩余 Base64 字符转换回 6 位值
    for (j = 0; j < 4; j++) {
      charArray4[j] = static_cast<unsigned char>(kBase64Chars.find(charArray4[j]));
    }

    // 解码剩余字符
    charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
    charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
    charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

    // 添加有效解码字节（忽略填充产生的 0 字节）
    for (j = 0; j < i - 1; j++) {
      ret += static_cast<std::string::value_type>(charArray3[j]);
    }
  }

  return ret;
}

std::string fromUrlSafe(const std::string& input) {
  std::string temp;
  temp.reserve(input.size() + 4);

  // 将 URL-safe Base64 字母表转换为标准 Base64
  for (const auto& c : input) {
    if (c == '-') {
      temp += '+';
    } else if (c == '_') {
      temp += '/';
    } else {
      temp += c;
    }
  }

  // 添加 padding
  if ((input.size() % 4) != 0u) {
    int toAppend = 4 - static_cast<int>(input.size() % 4);
    for (int i = 0; i < toAppend; i++) {
      temp += '=';
    }
  }

  return temp;
}

std::string toUrlSafe(const std::string& input) {
  std::string temp(input);

  // 移除 padding
  size_t found = temp.find_last_not_of('=');
  if (found == std::string::npos) return "";

  // 将标准 Base64 字母表转换为 URL-safe Base64
  utils::replaceAll(temp, "+", "-");
  utils::replaceAll(temp, "/", "_");

  return temp.substr(0, found + 1);
}

}  // namespace base64
}  // namespace codec
}  // namespace pickup
