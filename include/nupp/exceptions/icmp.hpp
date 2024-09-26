#ifndef NUPP_EXCEPTIONS_ICMP_HPP
#define NUPP_EXCEPTIONS_ICMP_HPP

#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/byteorder.hpp>
#include <nupp/exceptions/ip.hpp>
#include <nupp/exceptions/socket.hpp>
#include <nupp/export.hpp>

#include <fmt/base.h>

#include <cstdint>

namespace nupp {
namespace exceptions {

namespace icmp {

/**
 * @see https://datatracker.ietf.org/doc/html/rfc792
 */
struct NUPP_EXPORT message {
    std::uint8_t type;
    std::uint8_t code;
    beu16_t checksum = 0;
};

struct NUPP_EXPORT echo : public message {
    beu16_t identifier = 0;
    beu16_t sequence = 0;
    echo();
};

inline void before_send(icmp::message& data) {
    data.checksum = 0;
    data.checksum.raw() = ip::checksum(data);
}

}
}
}

#endif
