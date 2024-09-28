#ifndef NUPP_EXCEPTIONS_ICMP_HPP
#define NUPP_EXCEPTIONS_ICMP_HPP

#include <nupp/bytes_view.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/byteorder.hpp>
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
    std::uint8_t type;
    std::uint8_t code;
    beu16_t checksum = 0;
};

static_assert(std::is_trivially_destructible_v<message>);

struct NUPP_EXPORT echo : public message {
    beu16_t identifier = 0;
    beu16_t sequence = 0;

    echo();

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
struct echo_static : public echo {
    // sizeof(std::array<std::byte, N>) != sizeof(std::byte[N]) == N
    std::byte data[N];
};

static_assert(std::is_trivially_destructible_v<echo_static<8>>);

template <typename T>
requires std::derived_from<T, message>
void before_send(nupp::message<T>& data) {
    fmt::println("before_send({})", static_cast<void const*>(data.data()));
    fmt::println("before_send({})", static_cast<void const*>(data));
    data->checksum = 0;
    data->checksum.raw() = ip::checksum(data);
    fmt::println("before_send() =>\n{}", static_cast<bytes_view const&>(data));
}

}
}
}

#endif
