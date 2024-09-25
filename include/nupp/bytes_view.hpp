#ifndef NUPP_BYTES_VIEW_HPP
#define NUPP_BYTES_VIEW_HPP

#include <nupp/export.hpp>

#include <fmt/ostream.h>

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <span>

namespace nupp {

using bytes_view = std::span<std::byte const, std::dynamic_extent>;

template <typename T>
bytes_view as_bytes(T const& t) {
    return bytes_view{reinterpret_cast<std::byte const*>(&t), sizeof(T)};
}

template <typename T>
bool is_aligned(T const& t) {
    std::uintptr_t p = reinterpret_cast<std::uintptr_t>(&t);
    return p % alignof(T) == 0;
}

}

namespace std {

NUPP_EXPORT std::ostream& operator<< (std::ostream& out, nupp::bytes_view const bv);

}

template <>
struct NUPP_EXPORT fmt::formatter<nupp::bytes_view> : public ostream_formatter {};

#endif
