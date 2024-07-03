#include <nupp/exceptions/icmp.hpp>

#include <netinet/in.h>
#include <sys/socket.h>

namespace nupp {
namespace exceptions {

icmp_v4::icmp_v4(uint8_t type, uint8_t code) : socket_v4(SOCK_DGRAM, IPPROTO_ICMP) {
    _buffer.fill(0);
    message().icmp_type = type;
    message().icmp_code = code;
}

icmp_v4 icmp_v4::echo() {
    return icmp_v4(/*type=*/ICMP_ECHO, /*code=*/0);
}

proxy<uint16_t const> icmp_v4::id() const {
    return message().icmp_id;
}

proxy<uint16_t> icmp_v4::id() {
    return message().icmp_id;
}

proxy<uint16_t const> icmp_v4::seq() const {
    return message().icmp_seq;
}

proxy<uint16_t> icmp_v4::seq() {
    return message().icmp_seq;
}

}
}
