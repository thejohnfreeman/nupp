#include <nupp/exceptions/icmp.hpp>

#include <netinet/ip_icmp.h>

namespace nupp {
namespace exceptions {
namespace icmp {

// C++23: std::to_underlying()
#define ASSERT_EQ(lhs, rhs) \
    static_assert(static_cast<std::underlying_type_t<message_type_t>>(lhs) == rhs)

ASSERT_EQ(message_type_t::ECHO_REPLY, ICMP_ECHOREPLY);
ASSERT_EQ(message_type_t::ECHO, ICMP_ECHO);

echo_header::echo_header() : message{message_type_t::ECHO, /*code=*/0} {}

}
}
}
