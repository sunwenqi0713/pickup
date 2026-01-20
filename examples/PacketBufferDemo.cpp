#include <iostream>

#include "pickup/utils/PacketBuffer.h"

using namespace pickup::utils;

int main() {
  PacketBuffer pb;
  pb.write(123);
  pb.write(456);
  pb.write(789);
  std::cout << pb.read<int>() << std::endl;
  std::cout << pb.read<int>() << std::endl;
  std::cout << pb.read<int>() << std::endl;

//   // 写入数据
//   PacketBuffer buf;
//   buf.write<uint32_t>(0x12345678);
//   buf.write<double>(3.1415926);

//   // 读取数据
//   uint32_t magic = buf.read<uint32_t>();
//   double pi = buf.read<double>();

//   // 零拷贝访问
//   auto [begin, end] = buf.readableData();
//   process_raw_data(begin, end - begin);

//   // 处理网络粘包
//   while (buf.readableBytes() >= sizeof(uint32_t)) {
//     uint32_t pkt_len;
//     buf.peekBytes(&pkt_len, sizeof(pkt_len));

//     if (buf.readableBytes() >= pkt_len + sizeof(uint32_t)) {
//       buf.discardBytes(sizeof(uint32_t));  // 跳过长度头
//       process_packet(buf.readableData().first, pkt_len);
//       buf.discardBytes(pkt_len);
//     } else {
//       break;  // 等待更多数据
//     }
//   }

  return 0;
}