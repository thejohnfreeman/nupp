#include <nupp/bytes_view.hpp>

#include <fmt/base.h>

#include <cstdint>
#include <iostream>

template <typename T>
void print(char const* name) {
    fmt::println("{:16} {:4} {:4} \"{}\"", name, sizeof(T), alignof(T), T());
}

#define PRINT(T) print<T>(#T)

int main(int argc, const char** argv) {
    PRINT(std::uint64_t);
    PRINT(std::uint32_t);
    PRINT(std::uint16_t);
    PRINT(char);
    PRINT(unsigned char);
    PRINT(void*);
    PRINT(std::uintptr_t);
    int x = 45;
    void* p = static_cast<void*>(&x);
    fmt::println("{:18}", p);
    std::uintptr_t y = reinterpret_cast<std::uintptr_t>(p);
    fmt::println("{:x}", y);
    fmt::println("{}", reinterpret_cast<void*>(&p));
    fmt::println("{}", as_bytes(p));
    fmt::println("{}", as_bytes(p).subspan(2));
    return 0;
}
