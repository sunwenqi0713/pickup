#pragma once

#include <string>

namespace pickup {
namespace codec {

/// 将字符缓冲区编码为base64字符串
/**
 * @param input 输入数据指针
 * @param len 输入数据的字节长度
 * @return 表示输入数据的base64编码字符串
 */
std::string base64_encode(unsigned char const* input, size_t len);

/// 将字符串编码为base64字符串
/**
 * @param input 输入字符串
 * @return 表示输入数据的base64编码字符串
 */
std::string base64_encode(std::string const& input) {
  return base64_encode(reinterpret_cast<const unsigned char*>(input.data()), input.size());
}

/// 将base64编码的字符串解码为原始字节串
/**
 * @param input base64编码的输入数据
 * @return 表示解码后原始字节的字符串
 */
std::string base64_decode(std::string const& input);

}  // namespace codec
}  // namespace pickup