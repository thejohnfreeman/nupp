#include <nupp/exceptions/socket.hpp>

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> // close

#include <system_error>

namespace nupp {
namespace exceptions {

void socket_v4::bind(address_v4 const& address) {
  if (-1 == ::bind(_fd, (struct sockaddr*)&address, sizeof(address))) {
      throw std::system_error(errno, std::system_category());
  }
}

socket_v4::~socket_v4() {
    close(_fd);
}

socket_v4::socket_v4(int type, int protocol) {
    _fd = socket(AF_INET, type, protocol);
    if (_fd == -1) {
        throw std::system_error(errno, std::system_category());
    }
}

socket_v4 socket_v4::udp() {
    return socket_v4(/*type=*/SOCK_DGRAM, /*protocol=*/IPPROTO_UDP);
}

}
}
