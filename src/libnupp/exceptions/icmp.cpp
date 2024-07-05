#include <nupp/exceptions/icmp.hpp>
#include <nupp/exceptions/ip.hpp>

#include <netinet/in.h>
#include <sys/socket.h>

#include <system_error>

namespace nupp {
namespace exceptions {

icmp_v4::icmp_v4(uint8_t type, uint8_t code) : socket_v4(SOCK_DGRAM, IPPROTO_ICMP) {
    _buffer.fill(0);
    _message().icmp_type = type;
    _message().icmp_code = code;
}

icmp_v4 icmp_v4::echo() {
    return icmp_v4(/*type=*/ICMP_ECHO, /*code=*/0);
}

proxy<uint16_t const> icmp_v4::id() const {
    return _message().icmp_id;
}

proxy<uint16_t> icmp_v4::id() {
    return _message().icmp_id;
}

proxy<uint16_t const> icmp_v4::seq() const {
    return _message().icmp_seq;
}

proxy<uint16_t> icmp_v4::seq() {
    return _message().icmp_seq;
}

ssize_t icmp_v4::send(address_v4 const& address) {
    _message().icmp_cksum = 0;
    _message().icmp_cksum = ip_checksum(_buffer.data(), _buffer.size());
    return socket().send_to(_buffer, address);
}

}
}
