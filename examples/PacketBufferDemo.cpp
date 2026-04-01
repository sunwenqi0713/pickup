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

  // PacketBuffer 的参考用法（示例代码）:
  // PacketBuffer buf;
  // buf.write<uint32_t>(0x12345678);
  // buf.write<double>(3.1415926);
  //
  // uint32_t magic = buf.read<uint32_t>();
  // double pi = buf.read<double>();
  //
  // auto [begin, end] = buf.readableData();
  // process_raw_data(begin, end - begin);
  //
  // while (buf.readableBytes() >= sizeof(uint32_t)) {
  //   uint32_t pkt_len;
  //   buf.peekBytes(&pkt_len, sizeof(pkt_len));
  //   if (buf.readableBytes() >= pkt_len + sizeof(uint32_t)) {
  //     buf.discardBytes(sizeof(uint32_t));
  //     process_packet(buf.readableData().first, pkt_len);
  //     buf.discardBytes(pkt_len);
  //   } else {
  //     break;
  //   }
  // }

  return 0;
}