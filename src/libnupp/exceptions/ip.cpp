#include <nupp/exceptions/ip.hpp>

#include <cassert>

namespace nupp {
namespace exceptions {

uint16_t ip_checksum(void const* data, std::size_t size) {
  /* Data comes in bytes (8-bits). We want half-words (16-bits). */
  auto hwords = reinterpret_cast<uint16_t const*>(data);
  assert(size % 2 == 0);
  size >>= 1;

  uint32_t sum = 0;
  while (size--) {
    sum += *hwords++;
  }

  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  sum = ~sum;
  return static_cast<uint16_t>(sum);
}

}
}
