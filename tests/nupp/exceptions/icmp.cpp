#include <doctest/doctest.h>

#include <nupp/exceptions/icmp.hpp>

TEST_SUITE_BEGIN("icmp");

TEST_CASE("checksum") {
    using namespace nupp::exceptions;
    auto socket = socket_v4::icmp();
    socket.ttl() = 255;
    CHECK(socket.ttl() == 255);
    auto message = icmp::echo_message{};
    CHECK(message.sequence() == 0);
    message.sequence() = message.sequence() + 1;
    CHECK(message.sequence() == 1);
    set_checksum(message);
}

TEST_SUITE_END();
