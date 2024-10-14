#include <nupp/bytes.hpp>
#include <nupp/endian.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/icmp.hpp>
#include <nupp/exceptions/ip.hpp>

#include <fmt/base.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>

#define MEMORY(T) \
    fmt::println("{:16} {:4} {:4}", #T, sizeof(T), alignof(T))

#define ECHO(V) fmt::println("{} = {}", #V, V)

int main(int argc, const char** argv) {
    // MEMORY(std::uint64_t);
    // MEMORY(std::uint32_t);
    // MEMORY(std::uint16_t);
    // MEMORY(char);
    // MEMORY(unsigned char);
    // MEMORY(void*);
    // MEMORY(std::uintptr_t);
    // int x = 45;
    // void* p = static_cast<void*>(&x);
    // fmt::println("{:18}", p);
    // std::uintptr_t y = reinterpret_cast<std::uintptr_t>(p);
    // fmt::println("{:x}", y);
    // fmt::println("{}", reinterpret_cast<void*>(&p));
    // fmt::println("{}", nupp::to_bytes(p));
    // fmt::println("{}", nupp::to_bytes(p).subspan(2));
    // ECHO(ICMP_ECHO);

    // fmt::println("The answer is {answer}.", fmt::arg("answer", 42));
    // fmt::println("The answer is unknown.", fmt::arg("answer", 42));
    // fmt::println("The answer is unknown.", 42);

    // unsigned char buffer[17] = "abc\0efghijkl";
    // std::cout
    //     << nupp::pretty_printer{nupp::to_bytes(buffer)}
    //     << nupp::field<nupp::nu8_t>("VER / IHL")
    //     << nupp::field<nupp::nu8_t>("DSCP /ECN")
    //     << nupp::field<nupp::nu16_t>("length ({})")
    //     << nupp::field<nupp::nu16_t>("identification")
    //     << nupp::field<nupp::nu16_t>("flg / fragment offset")
    //     << nupp::field<nupp::exceptions::address32>("source address ({})")
    //     << nupp::end{};

    using namespace nupp::exceptions;

    {
        icmp::echo_fixed<8> request;
        request.identifier = 123;
        request.sequence = 1;
        std::cout << request;
    }

    {
        ip::packet<icmp::echo_fixed<8>> request;
        // std::cout << request;
    }

    return 0;
}
