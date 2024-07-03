#ifndef NUPP_EXCEPTIONS_ADDRESS_HPP
#define NUPP_EXCEPTIONS_ADDRESS_HPP

#include <nupp/export.hpp>

#include <sys/socket.h>
#include <netdb.h>

#include <system_error>
#include <type_traits>

namespace nupp {
namespace exceptions {

using address_v4 = struct sockaddr_in;

enum class NUPP_EXPORT address_err {
    ADDRFAMILY = EAI_ADDRFAMILY,
    AGAIN = EAI_AGAIN,
    BADFLAGS = EAI_BADFLAGS,
    FAIL = EAI_FAIL,
    FAMILY = EAI_FAMILY,
    MEMORY = EAI_MEMORY,
    NODATA = EAI_NODATA,
    NONAME = EAI_NONAME,
    SERVICE = EAI_SERVICE,
    SOCKTYPE = EAI_SOCKTYPE,
    SYSTEM = EAI_SYSTEM,
};

/**
 * @throws std::system_error
 */
NUPP_EXPORT address_v4 resolve_v4(char const* hostname);

}
}

namespace std {
    template <>
    struct is_error_code_enum<nupp::exceptions::address_err> : true_type {};
}

#endif
