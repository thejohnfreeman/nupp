#include <nupp/exceptions/socket.hpp>
#include <nupp/exceptions/icmp.hpp>

#include <doctest/doctest.h>

#include <array>
#include <vector>

TEST_SUITE_BEGIN("socket_v4");

TEST_CASE("udp()") {
    using namespace nupp::exceptions;
    auto socket = socket_v4::udp();
    socket.bind(address_v4::any());
    CHECK(true);
}

TEST_SUITE_END();

void compileSocket() {
    using namespace nupp::exceptions;

    auto dest = address_v4::of("google.com");
    auto socket = socket_v4::icmp();

    // We always know the size of a message before sending it.
    {
        // Construct a compile-time fixed-size message on the stack.
        icmp::echo_fixed<56> outgoing;
        // Send the message object.
        auto nsent = socket.send_to(outgoing, dest);
    }
    {
        // Construct a run-time fixed-size message in a buffer.
        std::byte buffer[1024] = {};
        nupp::message<icmp::echo> outgoing
            = nupp::message<icmp::echo>::construct(buffer, 64);
        // Send the message pointer.
        auto nsent = socket.send_to(outgoing, dest);
    }

    address_v4 src;

    // We do not always know the size of a message before receiving it.
    {
        // Construct a compile-time fixed-size message on the stack.
        icmp::echo_fixed<56> incoming;
        // Overwrite the message object.
        auto nreceived = socket.receive_from(incoming, src);
    }
    {
        // Construct a run-time fixed-size message in a buffer.
        std::byte buffer[1024] = {};
        nupp::message<icmp::echo> incoming
            = nupp::message<icmp::echo>::interpret(buffer, 64);
        // Overwrite the message pointer.
        auto nreceived = socket.receive_from(incoming, src);
    }
    {
        // Construct a buffer.
        std::byte buffer[1024] = {};
        // Overwrite the buffer, and return a message pointer.
        auto incoming = socket.receive_from<icmp::echo>(buffer, src);
        auto nreceived = incoming.size();
    }
    {
        // Buffer can be a std::array.
        std::array<std::byte, 1024> buffer;
        auto incoming = socket.receive_from<icmp::echo>(buffer, src);
        auto nreceived = incoming.size();
    }
    {
        // Buffer can be a std::vector.
        std::vector<std::byte> buffer(1024);
        auto incoming = socket.receive_from<icmp::echo>(buffer, src);
        auto nreceived = incoming.size();
    }

}
