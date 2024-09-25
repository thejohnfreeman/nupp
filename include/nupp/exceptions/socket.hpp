#ifndef NUPP_EXCEPTIONS_SOCKET_HPP
#define NUPP_EXCEPTIONS_SOCKET_HPP

#include <cstdint>
#include <nupp/exceptions/address.hpp>
#include <nupp/export.hpp>

#include <stdint.h> // uint32_t
#include <sys/socket.h>

#include <cassert>
#include <span>
#include <system_error>

namespace nupp {
namespace exceptions {

/**
 * An IPv4 socket.
 *
 * The constructor takes the same parameters as `socket()`,
 * except that the address family is fixed to IPv4.
 *
 * There are static factory methods for common socket types,
 * i.e. `udp()` and `tcp()`.
 *
 * Options can be read and written with the `opt()` member function.
 * It returns a reference proxy with an assignment operator (for set)
 * and a cast operator (for get).
 * Known options have named shortcut methods, e.g. `ttl()`.
 *
 * Remaining methods correspond to socket functions:
 *
 * - `bind()`
 * - `listen()`
 * - `accept()`
 * - `send()`
 * - `receive()`
 * - `send_to()`
 * - `receive_from()`
 */
class NUPP_EXPORT socket_v4 {
private:
    int _fd;

public:
    template <typename T>
    class option;

    /**
     * @param type e.g. `SOCK_STREAM` or `SOCK_DGRAM`.
     * @param protocol e.g. `IPPROTO_ICMP` or `IPPROTO_TCP`.
     * @throws std::system_error
     */
    socket_v4(int type, int protocol);
    ~socket_v4();

    int fd() const {
        return _fd;
    }

    /**
     * @throws std::system_error
     */
    void bind(address_v4 const& address);

    /**
     * @param level e.g. `IPPROTO_IP`.
     * @param name e.g. `IP_TTL`.
     */
    template <typename T>
    option<T> opt(int level, int name) {
        return option<T>(*this, level, name);
    }

    option<uint8_t> ttl();

    ssize_t send_to(std::span<uint8_t> const& data, address_v4 const& address);

    template <typename T>
    ssize_t send_to(T& data, address_v4 const& address) {
        before_send(data);
        return send_to({&data, sizeof(T)}, address);
    }

    static socket_v4 icmp();
    static socket_v4 tcp();
    static socket_v4 udp();

private:
    socket_v4(int fd) : _fd(fd) {}

    template <typename T>
    friend class option;
};

template <typename T>
void before_send(T& data) {}

// This must be a free function to permit third-party overloads.
template <typename T>
T recv_from(socket_v4 const& socket, address_v4& address);

template <typename T>
class NUPP_EXPORT socket_v4::option {
private:
    socket_v4& _socket;
    int _level;
    int _name;

public:
    option(socket_v4& socket, int level, int name)
        : _socket(socket)
          , _level(level)
          , _name(name) {}

    option& operator= (T const& x) {
        if (-1 == setsockopt(_socket._fd, _level, _name, &x, sizeof(T))) {
            throw std::system_error(errno, std::system_category());
        }
        return *this;
    }

    T value() const {
        T x;
        socklen_t length = sizeof(T);
        if (-1 == getsockopt(_socket._fd, _level, _name, &x, &length)) {
            throw std::system_error(errno, std::system_category());
        }
        assert(length == sizeof(T));
        return x;
    }

    operator T () const {
        return value();
    }
};

}
}

#endif
