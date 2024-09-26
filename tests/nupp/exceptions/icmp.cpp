#include <doctest/doctest.h>

#include <nupp/exceptions/icmp.hpp>

TEST_SUITE_BEGIN("icmp");

TEST_CASE("checksum") {
    using namespace nupp::exceptions;
    auto message = icmp::echo{};
    CHECK(message.sequence() == 0);
    message.sequence() = message.sequence() + 1;
    CHECK(message.sequence() == 1);
}

namespace {
    using namespace nupp::exceptions;
    static_assert(sizeof(ip::packet<icmp::echo>) == sizeof(ip::header) + 8);
}

TEST_SUITE_END();
