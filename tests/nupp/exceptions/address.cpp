#include <doctest/doctest.h>

#include <nupp/exceptions/address.hpp>

TEST_CASE("resolve_v4()") {
    using namespace nupp::exceptions;
    auto addr = resolve_v4("google.com");
    CHECK(true);
}
