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

/**
 * @see https://datatracker.ietf.org/doc/html/rfc792
 */
struct NUPP_EXPORT message {
    nu8_t type;
    nu8_t code;
    nu16_t checksum = 0;
};

static_assert(std::is_trivially_destructible_v<message>);

struct NUPP_EXPORT echo_header : public message {
    nu16_t identifier = 0;
    nu16_t sequence = 0;

    echo_header();

    // TODO: Could define cast operators for `::icmphdr` and `::icmp`,
    // but for whom? What functions expect them?

    template <std::size_t N = 0>
    rbytes<N> data() const {
        auto p = pointer_cast<std::byte const>(this);
        return {p + sizeof(echo_header), N};
    }

    rbytes<> data(std::size_t n) const {
        auto p = pointer_cast<std::byte const>(this);
        return {p + sizeof(echo_header), n};
    }

    template <std::size_t N = 0>
    wbytes<N> data() {
        auto p = pointer_cast<std::byte>(this);
        return {p + sizeof(echo_header), N};
    }

    wbytes<> data(std::size_t n) {
        auto p = pointer_cast<std::byte>(this);
        return {p + sizeof(echo_header), n};
    }

};

static_assert(std::is_trivially_destructible_v<echo_header>);

template <std::size_t N = 0>
struct echo : public echo_header {
    // sizeof(std::array<std::byte, N>) != sizeof(std::byte[N]) == N
    std::byte data[N];
};

static_assert(std::is_trivially_destructible_v<echo<8>>);

template <typename T>
requires std::derived_from<T, message>
void before_send(nupp::message<T>& data) {
    data->checksum = 0;
    data->checksum.raw() = ip::checksum(data);
}

}
}
}

#endif
