#ifndef NUPP_EXCEPTIONS_BYTEORDER_HPP
#define NUPP_EXCEPTIONS_BYTEORDER_HPP

#include <nupp/export.hpp>

#include <arpa/inet.h>

#include <bit>
#include <cassert>
#include <cstdint>
#include <span>

namespace nupp {
namespace exceptions {

inline uint16_t ntoh(uint16_t x) {
    return ntohs(x);
}

inline uint32_t ntoh(uint32_t x) {
    return ntohl(x);
}

inline uint16_t hton(uint16_t x) {
    return htons(x);
}

inline uint32_t hton(uint32_t x) {
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
    big_endian& operator= (T rhs) {
        _value = hton(rhs);
        return *this;
    }
    operator T() const {
        return ntoh(_value);
    }
    T raw() const {
        return _value;
    }
    T& raw() {
        return _value;
    }
};

using beu16_t = big_endian<uint16_t>;
static_assert(sizeof(beu16_t) == sizeof(uint16_t));
using beu32_t = big_endian<uint32_t>;
static_assert(sizeof(beu32_t) == sizeof(uint32_t));

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
    proxy& operator= (T rhs) {
        _x = hton(rhs);
        return *this;
    }
    operator T() const {
        return ntoh(_x);
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
