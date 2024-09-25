#include <nupp/exceptions/ip.hpp>

#include <cassert>

namespace nupp {
namespace exceptions {
namespace ip {

std::uint16_t checksum(bytes_view const& data) {
    /* Data comes in bytes (8-bits). We want half-words (16-bits). */
    auto size = data.size_bytes();
    assert(size % 2 == 0);
    auto hwords = reinterpret_cast<std::uint16_t const*>(data.data());
    size >>= 1;

    std::uint32_t sum = 0;
    while (size--) {
        sum += *hwords++;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    sum = ~sum;
    return static_cast<std::uint16_t>(sum);
}

}
}
}
