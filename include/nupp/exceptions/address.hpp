#ifndef NUPP_EXCEPTIONS_ADDRESS_HPP
#define NUPP_EXCEPTIONS_ADDRESS_HPP

#include <nupp/export.hpp>

#include <fmt/format.h>

#include <sys/socket.h>
#include <netdb.h>

#include <cstdint>
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
    static address_v4 from(std::uint32_t addr32);
    /**
     * @throws std::system_error
     */
    static address_v4 of(char const* hostname);
    static address_v4 any();
};

static_assert(sizeof(address_v4) == sizeof(sockaddr_in));

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

inline auto format_as(address_v4 const& address) {
    uint8_t const* octs = reinterpret_cast<uint8_t const*>(&address.sin_addr.s_addr);
    return fmt::format("{}.{}.{}.{}", octs[0], octs[1], octs[2], octs[3]);
}

}
}

namespace std {
    template <>
    struct is_error_code_enum<nupp::exceptions::address_err> : true_type {};
}

#endif
