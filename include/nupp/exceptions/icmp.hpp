#ifndef NUPP_EXCEPTIONS_ICMP_HPP
#define NUPP_EXCEPTIONS_ICMP_HPP

#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/byteorder.hpp>
#include <nupp/exceptions/ip.hpp>
#include <nupp/exceptions/socket.hpp>
#include <nupp/export.hpp>

#include <stdint.h>

namespace nupp {
namespace exceptions {
namespace icmp {

/**
 * @see https://datatracker.ietf.org/doc/html/rfc792
 */
struct NUPP_EXPORT message {
    uint8_t _type;
    uint8_t _code;
    uint16_t _checksum = 0;
};

struct NUPP_EXPORT echo_message : public message {
    uint16_t _identifier = 0;
    uint16_t _sequence = 0;

    echo_message();

    proxy<uint16_t> identifier() {
        return _identifier;
    }
    proxy<uint16_t const> identifier() const {
        return _identifier;
    }

    proxy<uint16_t> sequence() {
        return _sequence;
    }
    proxy<uint16_t const> sequence() const {
        return _sequence;
    }
};

template <typename T>
void set_checksum(T& message) {
    message._checksum = 0;
    message._checksum = ip_checksum(&message, sizeof(T));
}

}
}
}

#endif
