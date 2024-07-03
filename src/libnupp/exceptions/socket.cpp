#include <nupp/exceptions/socket.hpp>

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> // close

#include <cstring>
#include <system_error>

namespace nupp {
namespace exceptions {

void socket_v4::bind(uint32_t address) {
  struct sockaddr_in addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = address;
  if (-1 == ::bind(_fd, (struct sockaddr*)&addr, sizeof(addr))) {
      throw std::system_error(errno, std::system_category());
  }
}

socket_v4::~socket_v4() {
    close(_fd);
}

socket_v4 make_socket_v4(int type, int protocol) {
    int fd = socket(AF_INET, type, protocol);
    if (fd == -1) {
        throw std::system_error(errno, std::system_category());
    }
    return socket_v4(fd);
}

}
}
