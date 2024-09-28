#include <doctest/doctest.h>

#include <nupp/exceptions/byteorder.hpp>

static bool operator== (std::span<std::byte const> lhs, std::initializer_list<std::byte const> rhs) {
    assert(lhs.size() == rhs.size());
    auto l = lhs.begin();
    auto r = rhs.begin();
    while (l != lhs.end()) {
        if (*l != *r) {
            return false;
        }
        ++l;
        ++r;
    }
    return true;
}

TEST_SUITE_BEGIN("big_endian");

TEST_CASE("beu16_t") {
    using namespace nupp::exceptions;
    beu16_t x;
    CHECK(x == 0);
    x = 0xABCD;
    CHECK(x == 0xABCD);
    CHECK(x.raw() == 0xCDAB);
    // std::span<std::byte const, 2> z = {0xCD, 0xAB};
    // bool y = as_bytes(x.raw()) == z;
    // CHECK(as_bytes(x) == {0xCD, 0xAB});
}

TEST_SUITE_END();
