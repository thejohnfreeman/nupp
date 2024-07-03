#ifndef NUPP_EXCEPTIONS_SOCKET_HPP
#define NUPP_EXCEPTIONS_SOCKET_HPP

#include <nupp/exceptions/address.hpp>
#include <nupp/export.hpp>

#include <stdint.h> // uint32_t

namespace nupp {
namespace exceptions {

class NUPP_EXPORT socket_v4 {
private:
    int _fd;

public:
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

    static socket_v4 udp();

private:
    socket_v4(int fd) : _fd(fd) {}
};

}
}

#endif
