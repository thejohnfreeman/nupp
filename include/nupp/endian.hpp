#ifndef NUPP_ENDIAN_HPP
#define NUPP_ENDIAN_HPP

#include <nupp/export.hpp>

#include <fmt/base.h>

#include <arpa/inet.h>

#include <bit>
#include <cassert>
#include <concepts>
#include <cstdint>

namespace nupp {

inline std::uint16_t ntoh(std::uint16_t x) {
    return ntohs(x);
}

inline std::uint32_t ntoh(std::uint32_t x) {
    return ntohl(x);
}

inline std::uint16_t hton(std::uint16_t x) {
    return htons(x);
}

inline std::uint32_t hton(std::uint32_t x) {
    return htonl(x);
}

/**
 * A value that is big endian in memory regardless of native endianness,
 * but interacts with native values as if it were native endian.
 */
template <typename T>
class big_endian {
private:
    T _value;

public:
    big_endian() : _value(0) {}
    big_endian(T rhs) : _value(hton(rhs)) {}
    template <typename U>
    requires std::integral<U> && (sizeof(U) <= sizeof(T))
    big_endian& operator= (U rhs) {
        _value = hton(rhs);
        return *this;
    }
    T native() const {
        return ntoh(_value);
    }
    operator T() const {
        return native();
    }
    T raw() const {
        return _value;
    }
    T& raw() {
        return _value;
    }
};

template <typename T, typename U>
// TODO: Forbid narrowing conversions.
requires std::integral<U>
big_endian<T>& operator+= (big_endian<T>& lhs, U const& rhs) {
    return lhs = lhs.native() + rhs;
}

// TODO: More operators.

template <typename T>
auto format_as(big_endian<T> const& value) {
    return fmt::to_string(value.native());
}

using beu16_t = big_endian<std::uint16_t>;
static_assert(sizeof(beu16_t) == sizeof(std::uint16_t));
static_assert(alignof(beu16_t) == alignof(std::uint16_t));
using beu32_t = big_endian<std::uint32_t>;
static_assert(sizeof(beu32_t) == sizeof(std::uint32_t));
static_assert(alignof(beu32_t) == alignof(std::uint32_t));

/**
 * "n" prefix for "network order".
 * Uni-byte fields do not need a proxy.
 * Only multi-byte fields with a byte order,
 * or sub-byte fields with a bit mask, need a proxy.
 */
using nu8_t = std::uint8_t;
using nu16_t = beu16_t;
using nu32_t = beu32_t;

}

#endif
