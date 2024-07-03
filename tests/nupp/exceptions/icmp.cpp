#include <doctest/doctest.h>

#include <nupp/exceptions/icmp.hpp>

TEST_SUITE_BEGIN("icmp_v4");

TEST_CASE("seq()") {
    using namespace nupp::exceptions;
    auto icmp = icmp_v4::echo();
    icmp.seq() = 1;
}

TEST_SUITE_END();
