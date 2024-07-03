#ifndef NUPP_EXCEPTIONS_ADDRESS_HPP
#define NUPP_EXCEPTIONS_ADDRESS_HPP

#include <nupp/export.hpp>

#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h> // uint32_t

#include <system_error>
#include <type_traits>

namespace nupp {
namespace exceptions {

class NUPP_EXPORT address_v4 : public sockaddr_in {
public:
    address_v4();
    address_v4& operator= (sockaddr const& rhs);

    /**
     * @param address 32-bit address in network byte order
     */
    static address_v4 from(uint32_t addr32);
    /**
     * @throws std::system_error
     */
    static address_v4 of(char const* hostname);
    static address_v4 any();
};

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

}
}

namespace std {
    template <>
    struct is_error_code_enum<nupp::exceptions::address_err> : true_type {};
}

#endif
