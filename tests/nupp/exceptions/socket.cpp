#include <doctest/doctest.h>

#include <nupp/exceptions/socket.hpp>

#include <sys/socket.h>
#include <netinet/ip.h>

TEST_SUITE_BEGIN("socket_v4");

TEST_CASE("make_socket_v4()") {
    using namespace nupp::exceptions;
    auto socket = make_socket_v4(SOCK_DGRAM, IPPROTO_UDP);
    socket.bind(INADDR_ANY);
    CHECK(true);
}

TEST_SUITE_END();
