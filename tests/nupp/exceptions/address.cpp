#include <doctest/doctest.h>

#include <nupp/exceptions/address.hpp>

TEST_CASE("of()") {
    using namespace nupp::exceptions;
    auto addr = address_v4::of("google.com");
    CHECK(true);
}
