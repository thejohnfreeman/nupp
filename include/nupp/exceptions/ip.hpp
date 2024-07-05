#ifndef NUPP_EXCEPTIONS_IP_HPP
#define NUPP_EXCEPTIONS_IP_HPP

#include <nupp/export.hpp>

#include <stdint.h>

#include <cstddef>

namespace nupp {
namespace exceptions {

/**
 * @return checksum in network byte order
 */
NUPP_EXPORT uint16_t ip_checksum(void const* data, std::size_t size);

}
}

#endif
