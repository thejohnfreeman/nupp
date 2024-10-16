#ifndef NUPP_EXCEPTIONS_SOCKET_HPP
#define NUPP_EXCEPTIONS_SOCKET_HPP

#include <nupp/bytes.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/export.hpp>

#include <sys/socket.h>

#include <cassert>
#include <cstdint>
#include <span>
#include <system_error>

namespace nupp {
namespace exceptions {

namespace detail {

struct anything {
    template <typename T>
    anything() {}
};

// Require an implicit conversion to put this overload at the bottom.
// inline void before_send(anything const& data) {}

template <typename T>
void before_send(T& data) {
    nupp::message m(data);
    before_send(m);
}

}

enum class socket_type_t : int {
    DATAGRAM = SOCK_DGRAM,
    RAW = SOCK_RAW,
    STREAM = SOCK_STREAM,
};

enum class socket_protocol_t : int {
    ICMP = IPPROTO_ICMP,
    TCP = IPPROTO_TCP,
    UDP = IPPROTO_UDP,
};

using protocols = socket_protocol_t;

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
     * @throws std::system_error
     */
    socket_v4(socket_type_t type, socket_protocol_t protocol);
    ~socket_v4();

    int fd() const {
        return _fd;
    }

    /**
     * @param level e.g. `IPPROTO_IP`.
     * @param name e.g. `IP_TTL`.
     */
    template <typename T>
    option<T> opt(int level, int name) {
        return option<T>(*this, level, name);
    }

    option<std::uint8_t> ttl();

    /**
     * @throws std::system_error
     */
    void bind(address_v4 const& address = address_v4::any());

    /**
     * @throws std::system_error
     */
    void connect(address_v4 const& address);

    std::size_t _send_to(rbytes<> const& data, address_v4 const& address, unsigned int flags = 0);

    /** Send a dynamically-sized message of indirect type. */
    template <typename T>
    std::size_t send_to(nupp::message<T>& data, address_v4 const& address, unsigned int flags = 0) {
        using detail::before_send;
        before_send(data);
        return _send_to(data, address, flags);
    }

    /** Send a statically-sized message of direct type. */
    template <typename T>
    std::size_t send_to(T& data, address_v4 const& address, unsigned int flags = 0) {
        using detail::before_send;
        before_send(data);
        return _send_to(to_bytes(data), address, flags);
    }

    std::size_t _receive_from(wbytes<> const& data, address_v4& address, unsigned int flags = 0);

    /**
     * Write a dynamically-sized message to a buffer,
     * and interpret it as indirect type.
     */
    template <typename T>
    nupp::message<T> receive_from(wbytes<> const& data, address_v4& address, unsigned int flags = 0) {
        auto size = _receive_from(data, address, flags);
        return nupp::message<T>::interpret(data.data(), size);
    }

    /** Overwrite a dynamically-sized message of indirect type. */
    template <typename T>
    std::size_t receive_from(nupp::message<T>& data, address_v4& address, unsigned int flags = 0) {
        return _receive_from(data, address, flags);
    }

    /** Overwrite a statically-sized message of direct type. */
    template <typename T>
    std::size_t receive_from(T& data, address_v4& address, unsigned int flags = 0) {
        return _receive_from(to_bytes(data), address, flags);
    }

    static socket_v4 raw(socket_protocol_t protocol);
    static socket_v4 icmp();
    static socket_v4 tcp();
    static socket_v4 udp();

private:
    socket_v4(int fd) : _fd(fd) {}

    template <typename T>
    friend class option;
};

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
