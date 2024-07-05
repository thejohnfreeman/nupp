#include <nupp/exceptions/socket.hpp>

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> // close

#include <system_error>

namespace nupp {
namespace exceptions {

socket_v4::socket_v4(int type, int protocol) {
    _fd = socket(AF_INET, type, protocol);
    if (_fd == -1) {
        throw std::system_error(errno, std::system_category());
    }
}

socket_v4::~socket_v4() {
    close(_fd);
}

void socket_v4::bind(address_v4 const& address) {
  if (-1 == ::bind(_fd, (struct sockaddr*)&address, sizeof(address))) {
      throw std::system_error(errno, std::system_category());
  }
}

socket_v4::option<uint8_t> socket_v4::ttl() {
    return opt<uint8_t>(/*level=*/IPPROTO_IP, /*name=*/IP_TTL);
}

ssize_t socket_v4::send_to(
    std::span<uint8_t> const& data, address_v4 const& address
) {
    auto sent = ::sendto(
        _fd, data.data(), data.size(), /*flags=*/0,
        (sockaddr*)(&address), sizeof(address)
    );
    if (-1 == sent) {
        throw std::system_error(errno, std::system_category());
    }
    return sent;
}

socket_v4 socket_v4::icmp() {
    return socket_v4(/*type=*/SOCK_DGRAM, /*protocol=*/IPPROTO_ICMP);
}

socket_v4 socket_v4::tcp() {
    return socket_v4(/*type=*/SOCK_STREAM, /*protocol=*/IPPROTO_TCP);
}

socket_v4 socket_v4::udp() {
    return socket_v4(/*type=*/SOCK_DGRAM, /*protocol=*/IPPROTO_UDP);
}

}
}
