#include <nupp/bytes_view.hpp>

#include <fmt/base.h>

#include <netinet/ip_icmp.h>

#include <cstdint>
#include <iostream>

#define MEMORY(T) \
    fmt::println("{:16} {:4} {:4}", #T, sizeof(T), alignof(T))

#define ECHO(V) fmt::println("{} = {}", #V, V)

int main(int argc, const char** argv) {
    MEMORY(std::uint64_t);
    MEMORY(std::uint32_t);
    MEMORY(std::uint16_t);
    MEMORY(char);
    MEMORY(unsigned char);
    MEMORY(void*);
    MEMORY(std::uintptr_t);
    int x = 45;
    void* p = static_cast<void*>(&x);
    fmt::println("{:18}", p);
    std::uintptr_t y = reinterpret_cast<std::uintptr_t>(p);
    fmt::println("{:x}", y);
    fmt::println("{}", reinterpret_cast<void*>(&p));
    fmt::println("{}", nupp::to_bytes(p));
    fmt::println("{}", nupp::to_bytes(p).subspan(2));
    ECHO(ICMP_ECHO);
    return 0;
}
