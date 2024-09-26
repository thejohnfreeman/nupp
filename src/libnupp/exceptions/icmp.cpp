#include <nupp/exceptions/icmp.hpp>

#include <netinet/ip_icmp.h>

namespace nupp {
namespace exceptions {
namespace icmp {

echo::echo() : message{/*type=*/ICMP_ECHO, /*code=*/0} {}

}
}
}
