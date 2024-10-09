#include <nupp/exceptions/socket.hpp>

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> // close

#include <system_error>

namespace nupp {
namespace exceptions {

socket_v4::socket_v4(socket_type_t type, socket_protocol_t protocol) {
    // C++23: std::to_underlying
    _fd = ::socket(AF_INET, static_cast<int>(type), static_cast<int>(protocol));
    if (_fd == -1) {
        // TODO: is this the same as `strerror`?
        throw std::system_error(errno, std::system_category());
    }
}

socket_v4::~socket_v4() {
    ::close(_fd);
}

void socket_v4::bind(address_v4 const& address) {
    if (-1 == ::bind(_fd, reinterpret_cast<sockaddr const*>(&address), sizeof(address))) {
        throw std::system_error(errno, std::system_category());
    }
}

void socket_v4::connect(address_v4 const& address) {
    if (-1 == ::connect(_fd, address, sizeof(address))) {
        throw std::system_error(errno, std::system_category());
    }
}

socket_v4::option<std::uint8_t> socket_v4::ttl() {
    return opt<std::uint8_t>(/*level=*/IPPROTO_IP, /*name=*/IP_TTL);
}

std::size_t socket_v4::_send_to(
    rbytes<> const& data, address_v4 const& address, unsigned int flags
) {
    auto sent = ::sendto(
        _fd, data.data(), data.size(), flags, address, sizeof(address)
    );
    if (-1 == sent) {
        throw std::system_error(errno, std::system_category());
    }
    return sent;
}

std::size_t socket_v4::_receive_from(
    wbytes<> const& data, address_v4& address, unsigned int flags
) {
    socklen_t size = sizeof(address);
    auto received = ::recvfrom(
        _fd, data.data(), data.size(), flags, address, &size
    );
    if (sizeof(address) != size) {
        throw std::system_error(std::make_error_code(std::errc::message_size));
    }
    if (-1 == received) {
        throw std::system_error(errno, std::system_category());
    }
    return received;
}

socket_v4 socket_v4::raw(socket_protocol_t protocol) {
    return socket_v4(socket_type_t::RAW, protocol);
}

socket_v4 socket_v4::icmp() {
    return socket_v4(socket_type_t::DATAGRAM, socket_protocol_t::ICMP);
}

socket_v4 socket_v4::tcp() {
    return socket_v4(socket_type_t::STREAM, socket_protocol_t::TCP);
}

socket_v4 socket_v4::udp() {
    return socket_v4(socket_type_t::DATAGRAM, socket_protocol_t::UDP);
}

}
}
