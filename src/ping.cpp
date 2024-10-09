#include <nupp/bytes.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/icmp.hpp>

#include <fmt/base.h>
#include <fmt/chrono.h>

#include <unistd.h>     // close
#include <err.h>        // errno
#include <errno.h>      // errno
#include <netinet/ip_icmp.h> // ICMP_ECHOREPLY
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <cassert>
#include <chrono>
#include <cmath>   // sqrt
#include <csignal> // signal
#include <cstdio>  // fflush
#include <cstdlib> // atexit
#include <limits>  // max
#include <thread>  // sleep

using namespace nupp::exceptions;

const char* destname;
unsigned int nsent  = 0;
unsigned int nrecv  = 0;
double min   = std::numeric_limits<double>::max();
double max   = 0.0;
double total = 0.0;
// Sum of squares.
double sos   = 0.0;
std::chrono::steady_clock::time_point first, last;

void onexit() {
    std::chrono::duration<double, std::milli> ms = last - first;
    fmt::println("\n--- {} ping statistics ---", destname);
    fmt::println("{} packets transmitted, {} received, {:.0f}% packet loss, time {:.0}",
            nsent, nrecv, 100.0 * (nsent - nrecv) / nsent, ms);
    double mean   = total / nrecv;
    double stddev = std::sqrt((sos / nrecv) - (mean * mean));
    fmt::println("rtt min/avg/max/mdev = {:.3f}/{:.3f}/{:.3f}/{:.3f} ms",
            min, mean, max, stddev);
    std::fflush(nullptr);
}

void onsignal(int signal) {
    if (SIGINT == signal) exit(EXIT_SUCCESS);
}

constexpr std::size_t TOTAL_SIZE = 84;
constexpr std::size_t ICMP_SIZE = TOTAL_SIZE - sizeof(ip::header);
constexpr std::size_t DATA_SIZE = ICMP_SIZE - sizeof(icmp::echo);

// Send a statically-sized message of direct type.
struct sender1 {
    icmp::echo_fixed<DATA_SIZE> outgoing;

    icmp::echo& body = outgoing;
    auto send(socket_v4& socket, address_v4 const& dest) {
        return socket.send_to(outgoing, dest);
    }
};

// Send a dynamically-sized message of indirect type.
struct sender2 {
    std::byte buffer[1024] = {};
    nupp::message<icmp::echo> outgoing
        = nupp::message<icmp::echo>::construct(buffer, ICMP_SIZE);

    icmp::echo& body = *outgoing;
    auto send(socket_v4& socket, address_v4 const& dest) {
        return socket.send_to(outgoing, dest);
    }
};

// Overwrite a statically-sized message of direct type.
struct receiver1 {
    icmp::echo_fixed<DATA_SIZE> incoming;

    icmp::echo* body = &incoming;
    auto receive(socket_v4& socket, address_v4& src) {
        return socket.receive_from(incoming, src);
    }
};

// Overwrite a dynamically-sized message of indirect type.
struct receiver2 {
    std::byte buffer[1024] = {};
    nupp::message<icmp::echo> incoming
        = nupp::message<icmp::echo>::interpret(buffer, ICMP_SIZE);

    icmp::echo* body = incoming;
    auto receive(socket_v4& socket, address_v4& src) {
        return socket.receive_from(incoming, src);
    }

};

// Write a dynamically-sized message to a buffer,
// and interpret it as indirect type.
struct receiver3 {
    std::byte buffer[1024] = {};

    icmp::echo* body = nullptr;
    auto receive(socket_v4& socket, address_v4& src) {
        // nupp::message must be non-null.
        auto incoming = socket.receive_from<icmp::echo>(buffer, src);
        body = incoming;
        return incoming.size();
    }
};

int main(int argc, const char** argv) {

    /* Parse command line. */
    if (argc != 2) {
        fmt::println("usage: ping host");
        std::exit(EXIT_FAILURE);
    }

    destname = argv[1];

    auto dest = address_v4::of(destname);

    auto socket = socket_v4::icmp();
    socket.ttl() = 255;

    socket.bind();

    /* Schedule reporting. */
    if (SIG_ERR == std::signal(SIGINT, onsignal)) {
        exit(EXIT_FAILURE);
    }
    if (std::atexit(onexit)) {
        exit(EXIT_FAILURE);
    }

    /* Construct ICMP header. */
    sender2 sender;

    // Narrowing conversion from 32 to 16 bits.
    sender.body.identifier = static_cast<std::uint16_t>(getpid());
    sender.body.sequence = 1;

    fmt::println(
            "PING {} ({}) {}({}) bytes of data.",
            destname, dest, DATA_SIZE, TOTAL_SIZE);
    first = last = std::chrono::steady_clock::now();
    while (true) {
        /* Start timer. */
        auto start = std::chrono::steady_clock::now();

        /* Send. */
        auto sendbytes = sender.send(socket, dest);
        ++nsent;

        /* Receive. */
        address_v4 src;
        receiver3 receiver;
        auto recvbytes = receiver.receive(socket, src);

        ++nrecv;

        /* Verify. */
        assert(receiver.body->type == icmp::message_type_t::ECHO_REPLY);
        assert(receiver.body->code == 0);
        assert(receiver.body->sequence == sender.body.sequence);

        /* Stop timer. */
        auto stop = last = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> d = stop - start;
        auto ms = d.count();
        if (ms < min) min = ms;
        if (ms > max) max = ms;
        total += ms;
        sos += ms * ms;

        /* Print. */
        fmt::println(
                "{} bytes from {} ({}): icmp_seq={} ttl=xx time={:.2f} ms",
                sendbytes, src.name(), src, sender.body.sequence, ms);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        sender.body.sequence += 1;
    }

    return EXIT_SUCCESS;
}
