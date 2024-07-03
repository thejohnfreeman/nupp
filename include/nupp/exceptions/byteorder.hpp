#ifndef NUPP_EXCEPTIONS_BYTEORDER_HPP
#define NUPP_EXCEPTIONS_BYTEORDER_HPP

#include <nupp/export.hpp>

#include <arpa/inet.h>
#include <stdint.h>

namespace nupp {
namespace exceptions {

NUPP_EXPORT uint16_t ntoh(uint16_t x) {
    return ntohs(x);
}

NUPP_EXPORT uint32_t ntoh(uint32_t x) {
    return ntohl(x);
}

NUPP_EXPORT uint16_t hton(uint16_t x) {
    return htons(x);
}

NUPP_EXPORT uint32_t hton(uint32_t x) {
    return htonl(x);
}

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
    T operator() () const {
        return ntoh(_x);
    }
    T raw() const {
        return _x;
    }
    T& raw() {
        return _x;
    }
};

}
}

#endif
