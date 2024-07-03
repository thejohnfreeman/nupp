#ifndef NUPP_EXCEPTIONS_SOCKET_HPP
#define NUPP_EXCEPTIONS_SOCKET_HPP

#include <nupp/exceptions/address.hpp>
#include <nupp/export.hpp>

#include <stdint.h> // uint32_t
#include <sys/socket.h>

#include <system_error>

namespace nupp {
namespace exceptions {

class NUPP_EXPORT socket_v4 {
private:
    int _fd;

public:
    class option;

    /**
     * @param type e.g. `SOCK_STREAM` or `SOCK_DGRAM`.
     * @param protocol e.g. `IPPROTO_ICMP` or `IPPROTO_TCP`.
     * @throws std::system_error
     */
    socket_v4(int type, int protocol);
    ~socket_v4();

    /**
     * @throws std::system_error
     */
    void bind(address_v4 const& address);

    /**
     * @param level e.g. `IPPROTO_IP`.
     * @param name e.g. `IP_TTL`.
     */
    option opt(int level, int name);
    option ttl();

    static socket_v4 udp();

private:
    socket_v4(int fd) : _fd(fd) {}

    friend class option;
};

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

    template <typename T>
    option& operator= (T const& x) {
        if (-1 == setsockopt(_socket._fd, _level, _name, &x, sizeof(T))) {
            throw std::system_error(errno, std::system_category());
        }
        return *this;
    }
};

}
}

#endif
