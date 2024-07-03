#ifndef NUPP_EXCEPTIONS_SOCKET_HPP
#define NUPP_EXCEPTIONS_SOCKET_HPP

#include <nupp/export.hpp>

#include <stdint.h> // uint32_t

namespace nupp {
namespace exceptions {

class NUPP_EXPORT socket_v4 {
private:
    int _fd;

public:
    ~socket_v4();

    /**
     * @param address 32-bit address in network byte order
     * @throws std::system_error
     */
    void bind(uint32_t address);

private:
    socket_v4(int fd) : _fd(fd) {}

    friend socket_v4 make_socket_v4(int type, int protocol);
};

/**
 * @param type e.g. `SOCK_STREAM` or `SOCK_DGRAM`.
 * @param protocol e.g. `IPPROTO_ICMP` or `IPPROTO_TCP`.
 * @throws std::system_error
 */
NUPP_EXPORT socket_v4 make_socket_v4(int type, int protocol);

}
}

#endif
