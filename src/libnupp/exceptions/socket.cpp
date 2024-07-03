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

socket_v4 socket_v4::make(int type, int protocol) {
    int fd = socket(AF_INET, type, protocol);
    if (fd == -1) {
        throw std::system_error(errno, std::system_category());
    }
    return socket_v4(fd);
}

socket_v4 socket_v4::udp() {
    return make(SOCK_DGRAM, IPPROTO_UDP);
}

}
}
