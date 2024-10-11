#ifndef NUPP_EXCEPTIONS_IP_HPP
#define NUPP_EXCEPTIONS_IP_HPP

#include <nupp/bytes.hpp>
#include <nupp/endian.hpp>
#include <nupp/export.hpp>

#include <netinet/ip.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nupp {
namespace exceptions {
namespace ip {

/**
 * @return checksum in network byte order
 */
NUPP_EXPORT std::uint16_t checksum(rbytes<> const& data);

// Maximum size, in bytes, of the options field in IPv4 header. 
constexpr std::size_t MAXIMUM_OPTIONS_SIZE = 40;

// https://android.googlesource.com/platform/bionic/+/refs/heads/main/libc/include/netinet/ip.h#52
struct NUPP_EXPORT header {
    nu8_t ihl: 4;
    nu8_t version: 4;
    nu8_t service;
    nu16_t length;
    nu16_t id;
    nu16_t offset;
    nu8_t ttl;
    nu8_t protocol;
    nu16_t checksum;
    nu32_t source;
    nu32_t destination;

    header(std::uint8_t length = 5) : ihl(length) {}

    std::byte* address() {
        return pointer_cast<std::byte>(this);
    }

    std::byte const* address() const {
        return pointer_cast<std::byte const>(this);
    }

    std::size_t size() const {
        return ihl * sizeof(std::uint32_t);
    }

    template <std::size_t N = 0>
    rbytes<N> options() const {
        return {address() + sizeof(header), N};
    }

    rbytes<> options(std::size_t n) const {
        return {address() + sizeof(header), n};
    }

    template <std::size_t N = 0>
    wbytes<N> options() {
        return {address() + sizeof(header), N};
    }

    wbytes<> options(std::size_t n) {
        return {address() + sizeof(header), n};
    }

    template <std::size_t N>
    rbytes<N> payload() const {
        return {address() + size(), N};
    }

    rbytes<> payload() const {
        return {address() + size(), length - size()};
    }

    template <std::size_t N>
    wbytes<N> payload() {
        return {address() + size(), N};
    }

    wbytes<> payload() {
        return {address() + size(), length - size()};
    }

    // CAUTION: The payload of an IP packet
    // is not required to be a complete message.
    template <typename T>
    T& message() {
        return *pointer_cast<T>(address() + size());
    }

    template <typename T>
    T const& message() const {
        return *pointer_cast<T const>(address() + size());
    }

};

static_assert(std::is_trivially_destructible_v<header>);
static_assert(sizeof(header) == 5 * sizeof(std::uint32_t));
static_assert(sizeof(header) == sizeof(::ip));

template <std::size_t O = 0>
requires (O <= MAXIMUM_OPTIONS_SIZE)
struct NUPP_EXPORT header_fixed : public header {
    static constexpr std::size_t noptions = O;
    // An options field.
    std::byte options[noptions * sizeof(std::uint32_t)];
    header_fixed() : header(5 + noptions) {}
};

template <>
class NUPP_EXPORT header_fixed<0> : public header {
public:
    static constexpr std::size_t noptions = 0;
    // Exclude the `options` member
    // or else it will consume bytes for a unique address.
    header_fixed() : header(5 + noptions) {}
};

template <typename T, std::size_t O = 0>
requires (O <= MAXIMUM_OPTIONS_SIZE)
struct NUPP_EXPORT packet : public header_fixed<O> {
    T payload;
};

}
}
}

#endif
