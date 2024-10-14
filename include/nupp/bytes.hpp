#ifndef NUPP_BYTES_HPP
#define NUPP_BYTES_HPP

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
// `std::span` is missing comparison operators,
// which makes us define our own,
// but it is also missing stream operators,
// which _lets_ us define our own.
// https://www.reddit.com/r/cpp/comments/fg79hm/differences_between_stdstring_view_and_stdspan/
template <std::size_t N = std::dynamic_extent>
using rbytes = std::span<std::byte const, N>;

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

inline auto to_bytes(rbytes<> const& bytes) {
    return bytes;
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

struct NUPP_EXPORT pretty_printer {
    nupp::rbytes<> bytes;
    template <typename T>
    pretty_printer(T const& object) : bytes(to_bytes(object)) {}
};

template <typename T>
struct NUPP_EXPORT field {
    fmt::format_string<T> fstring;
};

template <typename T>
struct NUPP_EXPORT fields {
    T const* address = nullptr;
    operator T const* () const { return address; }
};

struct NUPP_EXPORT end {};

struct NUPP_EXPORT pstream {
    nupp::rbytes<> bytes;
    std::ostream& out;
    mutable std::size_t index = 0;

    void intro() const;
    void line() const;
    void print(char* buffer, std::size_t nbytes) const;
    std::ostream& outro() const;
};

namespace detail {

/** Maximum width of a formatted description. */
template <std::size_t N> struct NUPP_EXPORT field_width;
template <> struct NUPP_EXPORT field_width< 8> { static constexpr std::size_t WIDTH = 9; };
template <> struct NUPP_EXPORT field_width<16> { static constexpr std::size_t WIDTH = 21; };
template <> struct NUPP_EXPORT field_width<32> { static constexpr std::size_t WIDTH = 45; };

template <typename T>
struct NUPP_EXPORT field_traits {
    using value_type = T;
    static constexpr std::size_t NBYTES = sizeof(T) / sizeof(std::uint8_t);
    static constexpr std::size_t NBITS = NBYTES << 3;
    static constexpr std::size_t WIDTH = field_width<NBITS>::WIDTH;
};

template <typename... T>
static char* cformat(char* out, fmt::format_string<T...> fmt, T&&... args) {
    auto end = fmt::format_to(out, fmt, std::forward<T>(args)...);
    *end = 0;
    return end;
}

}

template <typename T>
pstream const& operator<< (pstream const& out, field<T> const& f) {
    using trait = detail::field_traits<T>;
    T const& value = *reinterpret_cast<T const*>(out.bytes.data() + out.index);
    char buffer1[trait::WIDTH + 1];
    // TODO: Null-terminated C-strings or std::string_views?
    auto end = detail::cformat(buffer1, fmt::runtime(f.fstring), fmt::arg("value", value));
    // TODO: replace with better exception
    assert(end - buffer1 <= trait::WIDTH);
    char buffer2[trait::WIDTH + 3 + 1];
    detail::cformat(buffer2, "| {:^{}} ", buffer1, trait::WIDTH);
    out.print(buffer2, trait::NBYTES);
    return out;
}

inline std::ostream& operator<< (pstream const& out, end) {
    return out.outro();
}

inline pstream operator<< (std::ostream& out, pretty_printer const& pp) {
    pstream pout{pp.bytes, out};
    pout.intro();
    return pout;
}

}

namespace std {

NUPP_EXPORT std::ostream& operator<< (std::ostream& out, nupp::rbytes<> const& bv);

template <typename T>
requires std::convertible_to<T, nupp::rbytes<>>
std::ostream& operator<< (std::ostream& out, T const& object) {
    return out << static_cast<nupp::rbytes<> const&>(object);
}

}

template <std::size_t N>
struct NUPP_EXPORT fmt::formatter<nupp::rbytes<N>> : public ostream_formatter {};

template <std::size_t N>
struct NUPP_EXPORT fmt::formatter<nupp::wbytes<N>> : public ostream_formatter {};

template <typename T>
struct NUPP_EXPORT fmt::formatter<nupp::message<T>> : public ostream_formatter {};

#endif
