#ifndef NUPP_BYTES_VIEW_HPP
#define NUPP_BYTES_VIEW_HPP

#include <nupp/export.hpp>

#include <fmt/ostream.h>

#include <cstddef>
#include <ostream>
#include <span>

using bytes_view = std::span<std::byte const, std::dynamic_extent>;

namespace std {

NUPP_EXPORT std::ostream& operator<< (std::ostream& out, bytes_view const bv);

}

template <>
struct NUPP_EXPORT fmt::formatter<bytes_view> : public ostream_formatter {};

template <typename T>
bytes_view as_bytes(T const& t) {
    return bytes_view{reinterpret_cast<std::byte const*>(&t), sizeof(T)};
}

#endif
