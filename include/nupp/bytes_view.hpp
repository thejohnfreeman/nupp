#ifndef NUPP_BYTES_VIEW_HPP
#define NUPP_BYTES_VIEW_HPP

#include <nupp/export.hpp>

#include <fmt/ostream.h>

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <span>
#include <type_traits>

namespace nupp {

// Read-only byte sequence.
template <std::size_t N = std::dynamic_extent>
using rbytes = std::span<std::byte const, N>;
using bytes_view = rbytes<>;

// Writeable byte sequence.
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
bool is_aligned(void const* address) {
    std::uintptr_t p = reinterpret_cast<std::uintptr_t>(address);
    return p % alignof(T) == 0;
}

template <typename T>
bool is_aligned(T* address) {
    return is_aligned<T>(static_cast<void const*>(address));
}

template <typename T>
requires (!std::convertible_to<T, void const*>)
bool is_aligned(T const& object) {
    return is_aligned(&object);
}

/**
 * A light pointer to a runtime-sized message.
 */
template <typename T>
requires std::is_trivially_destructible_v<T>
class message : public wbytes<> {
public:
    using type = T;

    message(T& object) : message(&object, sizeof(T)) {}

    template <typename U = T>
    requires std::is_trivially_destructible_v<U>
    static message<U> construct(void* pointer, std::size_t size) {
        new (pointer) T();
        return {pointer, size};
    }

    static message<T> interpret(void* pointer, std::size_t size) {
        return {pointer, size};
    }

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

private:
    message(void* pointer, std::size_t size)
        : wbytes<>(static_cast<std::byte*>(pointer), size)
    {
        assert(size >= sizeof(T));
        assert(is_aligned<T>(pointer));
    }

};

}

namespace std {

NUPP_EXPORT std::ostream& operator<< (std::ostream& out, nupp::bytes_view const& bv);

template <typename T>
requires std::convertible_to<T, nupp::bytes_view>
std::ostream& operator<< (std::ostream& out, T const& object) {
    return out << static_cast<nupp::bytes_view const&>(object);
}

}

template <std::size_t N>
struct NUPP_EXPORT fmt::formatter<nupp::rbytes<N>> : public ostream_formatter {};

template <>
struct NUPP_EXPORT fmt::formatter<nupp::bytes_view> : public ostream_formatter {};

template <std::size_t N>
struct NUPP_EXPORT fmt::formatter<nupp::wbytes<N>> : public ostream_formatter {};

template <typename T>
struct NUPP_EXPORT fmt::formatter<nupp::message<T>> : public ostream_formatter {};

#endif
