#include <nupp/exceptions/icmp.hpp>

#include <nupp/exceptions/ip.hpp>

#include <netinet/ip_icmp.h> // ICMP_ECHO

namespace nupp {
namespace exceptions {
namespace icmp {

echo::echo() : message{/*type=*/ICMP_ECHO, /*code=*/0} {}

void before_send(icmp::message& data) {
    data.checksum = 0;
    data.checksum.raw() = ip::checksum(to_bytes(data));
}

}
}
}
