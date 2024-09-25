#ifndef NUPP_EXCEPTIONS_BYTEORDER_HPP
#define NUPP_EXCEPTIONS_BYTEORDER_HPP

#include <nupp/export.hpp>

#include <arpa/inet.h>

#include <bit>
#include <cassert>
#include <concepts>
#include <cstdint>

namespace nupp {
namespace exceptions {

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

using beu16_t = big_endian<std::uint16_t>;
static_assert(sizeof(beu16_t) == sizeof(std::uint16_t));
using beu32_t = big_endian<std::uint32_t>;
static_assert(sizeof(beu32_t) == sizeof(std::uint32_t));

// TODO: format_as(big_endian)

/**
 * A proxy for network byte order fields.
 * Holds a reference to a network byte order value.
 * Cast (read) and assignment (write) operators
 * convert to and from host byte order, resepectively.
 */
template <typename T>
class proxy {
private:
    T& _x;
public:
    proxy(T& x) : _x(x) {}
    // Forbid narrowing conversions.
    template <typename U>
    requires std::integral<U> && (sizeof(U) <= sizeof(T))
    proxy& operator= (U rhs) {
        _x = hton(rhs);
        return *this;
    }
    T native() const {
        return ntoh(_x);
    }
    operator T() const {
        return native();
    }
    T raw() const {
        return _x;
    }
    T& raw() {
        return _x;
    }
};

#define PROXY(type, name, member) \
    proxy<type> name() { \
        return member; \
    } \
    proxy<type const> name() const { \
        return member; \
    } \

}
}

#endif
