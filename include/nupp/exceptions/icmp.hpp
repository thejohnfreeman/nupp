#ifndef NUPP_EXCEPTIONS_ICMP_HPP
#define NUPP_EXCEPTIONS_ICMP_HPP

#include <nupp/bytes.hpp>
#include <nupp/endian.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/ip.hpp>
#include <nupp/exceptions/socket.hpp>
#include <nupp/export.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace nupp {
namespace exceptions {
namespace icmp {

enum class message_type_t : std::uint8_t {
    ECHO_REPLY = 0,
    ECHO = 8,
};
static_assert(sizeof(message_type_t) == sizeof(nu8_t));

/**
 * @see https://datatracker.ietf.org/doc/html/rfc792
 */
struct NUPP_EXPORT header {
    message_type_t type;
    nu8_t code;
    nu16_t checksum = 0;
};

static_assert(std::is_trivially_destructible_v<header>);

struct NUPP_EXPORT echo : public header {
    nu16_t identifier = 0;
    nu16_t sequence = 0;

    echo();

    // TODO: Could define cast operators for `::icmphdr` and `::icmp`,
    // but for whom? What functions expect them?

    template <std::size_t N = 0>
    rbytes<N> data() const {
        auto p = pointer_cast<std::byte const>(this);
        return {p + sizeof(echo), N};
    }

    rbytes<> data(std::size_t n) const {
        auto p = pointer_cast<std::byte const>(this);
        return {p + sizeof(echo), n};
    }

    template <std::size_t N = 0>
    wbytes<N> data() {
        auto p = pointer_cast<std::byte>(this);
        return {p + sizeof(echo), N};
    }

    wbytes<> data(std::size_t n) {
        auto p = pointer_cast<std::byte>(this);
        return {p + sizeof(echo), n};
    }

};

static_assert(std::is_trivially_destructible_v<echo>);

template <std::size_t N = 0>
struct echo_fixed : public echo {
    // sizeof(std::array<std::byte, N>) != sizeof(std::byte[N]) == N
    std::byte data[N];
};

static_assert(std::is_trivially_destructible_v<echo_fixed<8>>);

template <typename T>
requires std::derived_from<T, header>
void before_send(nupp::message<T>& data) {
    data->checksum = 0;
    data->checksum.raw() = ip::checksum(data);
}

}
}
}

#endif
