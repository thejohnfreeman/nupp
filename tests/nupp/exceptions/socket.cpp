#include <doctest/doctest.h>

#include <nupp/exceptions/socket.hpp>

TEST_SUITE_BEGIN("socket_v4");

TEST_CASE("udp()") {
    using namespace nupp::exceptions;
    auto socket = socket_v4::udp();
    socket.bind(address_v4::any());
    CHECK(true);
}

TEST_SUITE_END();
