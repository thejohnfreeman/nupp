#ifndef NUPP_EXCEPTIONS_ADDRESS_HPP
#define NUPP_EXCEPTIONS_ADDRESS_HPP

#include <nupp/export.hpp>

#include <fmt/format.h>

#include <sys/socket.h>
#include <netdb.h>

#include <cstdint>
#include <string>
#include <system_error>
#include <type_traits>

namespace nupp {
namespace exceptions {

/**
 * An `address_v4` _is_ a `sockaddr_in`
 * so that it can be passed to the same functions.
 */
class NUPP_EXPORT address_v4 : public sockaddr_in {
public:
    address_v4();
    address_v4& operator= (sockaddr const& rhs);

    operator sockaddr const* () const {
        return reinterpret_cast<sockaddr const*>(this);
    }

    operator sockaddr* () {
        return reinterpret_cast<sockaddr*>(this);
    }

    std::string name() const;

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

inline auto format_as(address_v4 const& address) {
    std::uint8_t const* octs = reinterpret_cast<std::uint8_t const*>(&address.sin_addr.s_addr);
    return fmt::format("{}.{}.{}.{}", octs[0], octs[1], octs[2], octs[3]);
}

}
}

#endif
