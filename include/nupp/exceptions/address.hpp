#ifndef NUPP_EXCEPTIONS_ADDRESS_HPP
#define NUPP_EXCEPTIONS_ADDRESS_HPP

#include <nupp/bytes.hpp>
#include <nupp/export.hpp>

#include <fmt/format.h>

#include <netinet/in.h>

#include <cstdint>
#include <string>
#include <system_error>
#include <type_traits>

namespace nupp {
namespace exceptions {

/**
 * An `address32` wraps a 32-bit IPv4 address in network byte order (big-endian).
 */
struct NUPP_EXPORT address32 {
    std::uint8_t octets[4];
};

inline auto format_as(address32 address) {
    auto const& oct = address.octets;
    return fmt::format("{}.{}.{}.{}", oct[0], oct[1], oct[2], oct[3]);
}

/**
 * An `address_v4` _is_ a `sockaddr_in`
 * so that it can be passed to the same functions.
 */
struct NUPP_EXPORT address_v4 : public sockaddr_in {
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
     * Parses dot notation or domain name.
     * @throws std::system_error
     */
    static address_v4 of(char const* hostname);
    static address_v4 parse(char const* notation);
    static address_v4 any();
};

static_assert(sizeof(address_v4) == sizeof(sockaddr_in));

inline auto format_as(address_v4 const& address) {
    address32 const* addr = pointer_cast<address32>(&address.sin_addr.s_addr);
    return fmt::format("{}", *addr);
}

}
}

#endif
