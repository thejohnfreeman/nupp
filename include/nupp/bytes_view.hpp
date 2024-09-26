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

template <typename U, typename T>
U const* pointer_cast(T const* p) {
    // Three-point turn around reinterpret_cast.
    return static_cast<U const*>(static_cast<void const*>(p));
}

template <typename U, typename T>
U* pointer_cast(T* p) {
    return static_cast<U*>(static_cast<void*>(p));
}

template <typename T>
bytes_view to_bytes(T const& object) {
    return bytes_view{pointer_cast<std::byte>(&object), sizeof(T)};
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
