#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/icmp.hpp>
#include <nupp/exceptions/socket.hpp>

#include <fmt/base.h>
#include <unistd.h> // getpid

#include <cstdlib>

int main(int argc, const char** argv) {

    /* Parse command line. */
    if (argc != 2) {
        fmt::println("usage: {} host", argv[0]);
        std::exit(EXIT_FAILURE);
    }

    using namespace nupp::exceptions;

    auto dest = address_v4::of(argv[1]);
    auto socket = socket_v4::raw(protocols::ICMP);
    socket.bind();

    icmp::echo_fixed<0> request;
    request.identifier = static_cast<std::uint16_t>(getpid());
    request.sequence = 1;
    std::byte buffer[1024] = {};
    address_v4 src;

    for (int distance = 1;; ++distance) {
        socket.ttl() = distance;
        socket.send_to(request, dest);
        auto response = socket.receive_from<ip::header>(buffer, src);
        fmt::println("{}", response);
        break;
    }

    return 0;
}
