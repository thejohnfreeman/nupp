#ifndef NUPP_EXCEPTIONS_ICMP_HPP
#define NUPP_EXCEPTIONS_ICMP_HPP

#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/byteorder.hpp>
#include <nupp/exceptions/socket.hpp>
#include <nupp/export.hpp>

#include <netinet/ip_icmp.h>
#include <stdint.h>
#include <sys/types.h>

#include <array>

namespace nupp {
namespace exceptions {

/**
 * @see https://android.googlesource.com/platform/bionic/+/master/libc/include/netinet/ip_icmp.h#65
 */
class NUPP_EXPORT icmp_v4 : private socket_v4 {
private:
    // TODO: Fix alignment.
    std::array<uint8_t, ICMP_MINLEN> _buffer;

public:
    /**
     * @param type e.g. `ICMP_ECHO`.
     * @param code ???
     */
    icmp_v4(uint8_t type, uint8_t code);

    socket_v4& socket() {
        return *this;
    }
    socket_v4 const& socket() const {
        return *this;
    }

    proxy<uint16_t const> id() const;
    proxy<uint16_t> id();

    proxy<uint16_t const> seq() const;
    proxy<uint16_t> seq();

    ssize_t send(address_v4 const& address);

    static icmp_v4 echo();

private:
    ::icmp& _message() {
        // TODO: Does the standard guarantee this is safe?
        return *reinterpret_cast<::icmp*>(_buffer.data());
    }
    ::icmp const& _message() const{
        return *reinterpret_cast<::icmp const*>(_buffer.data());
    }
};

}
}

#endif
