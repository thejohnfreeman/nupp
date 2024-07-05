#include <nupp/exceptions/icmp.hpp>

#include <netinet/ip_icmp.h>

namespace nupp {
namespace exceptions {
namespace icmp {

echo_message::echo_message() : message{/*type=*/ICMP_ECHO, /*code=*/0} {}

}
}
}
