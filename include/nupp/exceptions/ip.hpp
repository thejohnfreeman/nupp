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

template <typename T>
struct payload_traits {
    static constexpr std::size_t size = sizeof(T);
};

template <std::size_t N>
concept is_dynamic_size = N == std::dynamic_extent;

template <std::size_t N>
concept is_static_size = !is_dynamic_size<N>;

template <typename T>
concept is_dynamic_type = is_dynamic_size<payload_traits<T>::size>;

template <typename T>
concept is_static_type = !is_dynamic_type<T>;

// https://android.googlesource.com/platform/bionic/+/refs/heads/main/libc/include/netinet/ip.h#52
struct header {
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
};

static_assert(sizeof(header) == 5 * 4);
static_assert(sizeof(header) == sizeof(::ip));

template <std::size_t O = 0>
requires is_static_size<O>
struct headerplus : public header {
    static constexpr std::size_t noptions = O;

    std::byte options[noptions * 4];
};

template <>
class headerplus<0> : public header {
public:
    static constexpr std::size_t noptions = 0;
};

template <typename T, std::size_t O = 0>
class packet {
private:
    std::vector<std::byte> _data;

protected:
    void* offset(std::size_t i) {
        return _data.data() + i;
    }
    void const* offset(std::size_t i) const {
        return _data.data() + i;
    }

public:
    ip::header& header() {
        return *reinterpret_cast<ip::header*>(offset(0));
    }
    ip::header const& header() const {
        return *reinterpret_cast<ip::header const*>(offset(0));
    }

    T& payload() {
        return *reinterpret_cast<T*>(offset(header().ip_hl * 4));
    }
    T const& payload() const {
        return *reinterpret_cast<T const*>(offset(header().ip_hl * 4));
    }

    operator T& () {
        return payload();
    }
    operator T const& () const {
        return payload();
    }
};

template <is_static_type T, std::size_t O>
requires is_static_size<O>
class packet<T, O> : public headerplus<O> {
private:
    T _payload;

public:
    ip::header& header() {
        return *this;
    }
    ip::header const& header() const {
        return *this;
    }

    T& payload() {
        return _payload;
    }
    T const& payload() const {
        return _payload;
    }

    operator T& () {
        return payload();
    }
    operator T const& () const {
        return payload();
    }
};

}
}
}

#endif
