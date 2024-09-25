#ifndef NUPP_BYTES_VIEW_HPP
#define NUPP_BYTES_VIEW_HPP

#include <nupp/export.hpp>

#include <fmt/ostream.h>

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <span>

namespace nupp {

using bytes_view = std::span<std::byte const>;

template <typename T>
bytes_view to_bytes(T const& object) {
    // Three-point turn around reinterpret_cast.
    auto p1 = &object;
    auto p2 = static_cast<void const*>(p1);
    auto p3 = static_cast<std::byte const*>(p2);
    return bytes_view{p3, sizeof(T)};
}

template <typename T>
bool is_aligned(T const& object) {
    std::uintptr_t p = reinterpret_cast<std::uintptr_t>(&object);
    return p % alignof(T) == 0;
}

}

namespace std {

NUPP_EXPORT std::ostream& operator<< (std::ostream& out, nupp::bytes_view const bv);

}

template <>
struct NUPP_EXPORT fmt::formatter<nupp::bytes_view> : public ostream_formatter {};

#endif
