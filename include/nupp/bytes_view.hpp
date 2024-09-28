#ifndef NUPP_BYTES_VIEW_HPP
#define NUPP_BYTES_VIEW_HPP

#include <nupp/export.hpp>

#include <fmt/ostream.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <span>

namespace nupp {

template <std::size_t N = std::dynamic_extent>
using rbytes = std::span<std::byte const, N>;
using bytes_view = rbytes<>;

template <std::size_t N = std::dynamic_extent>
using wbytes = std::span<std::byte, N>;

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
auto to_bytes(T const& object) {
    return rbytes<sizeof(T)>(pointer_cast<std::byte>(&object), sizeof(T));
}

template <std::size_t N, typename T>
auto to_bytes(T const& object) {
    return rbytes<N>(pointer_cast<std::byte>(&object), N);
}

template <typename T>
auto to_bytes(T& object) {
    return wbytes<sizeof(T)>(pointer_cast<std::byte>(&object), sizeof(T));
}

template <typename T>
bool is_aligned(T const& object) {
    std::uintptr_t p = reinterpret_cast<std::uintptr_t>(&object);
    return p % alignof(T) == 0;
}

/**
 * A light pointer to a runtime-sized message.
 */
template <typename T>
struct message : public wbytes<> {
    using type = T;

    message(void* pointer, std::size_t size)
        : wbytes<>(static_cast<std::byte*>(pointer), size)
    {
        assert(size >= sizeof(T));
    }

    message(T& object) : message(&object, sizeof(T)) {}

    T& operator* () {
        auto& ref = *pointer_cast<T>(data());
        assert(is_aligned(ref));
        return ref;
    }

    T* operator-> () {
        return &**this;
    }

    operator T* () {
        return &**this;
    }

    T const& operator* () const {
        auto const& ref = *pointer_cast<T const>(data());
        assert(is_aligned(ref));
        return ref;
    }

    T const* operator-> () const {
        return &**this;
    }

    operator T const* () const {
        return &**this;
    }

};

}

namespace std {

NUPP_EXPORT std::ostream& operator<< (std::ostream& out, nupp::bytes_view const bv);

}

template <std::size_t N>
struct NUPP_EXPORT fmt::formatter<nupp::rbytes<N>> : public ostream_formatter {};

template <>
struct NUPP_EXPORT fmt::formatter<nupp::bytes_view> : public ostream_formatter {};

template <std::size_t N>
struct NUPP_EXPORT fmt::formatter<nupp::wbytes<N>> : public ostream_formatter {};

#endif
