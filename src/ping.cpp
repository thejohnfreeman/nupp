#include <nupp/bytes.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/icmp.hpp>
#include <nupp/exceptions/socket.hpp>

#include <fmt/base.h>
#include <fmt/chrono.h>

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

constexpr std::size_t DATA_SIZE = 56;
constexpr std::size_t ICMP_SIZE = DATA_SIZE + sizeof(icmp::echo);
constexpr std::size_t IP_SIZE = ICMP_SIZE + sizeof(ip::header);

int main(int argc, const char** argv) {

    /* Parse command line. */
    if (argc != 2) {
        fmt::println("usage: {} host", argv[0]);
        std::exit(EXIT_FAILURE);
    }

    destname = argv[1];

    auto dest = address_v4::of(destname);

    auto socket = socket_v4::raw(protocols::ICMP);
    socket.ttl() = 255;

    socket.bind();

    /* Schedule reporting. */
    if (SIG_ERR == std::signal(SIGINT, onsignal)) {
        exit(EXIT_FAILURE);
    }
    if (std::atexit(onexit)) {
        exit(EXIT_FAILURE);
    }

    /* Construct ICMP message. */
    icmp::echo_fixed<DATA_SIZE> request;

    // Narrowing conversion from 32 to 16 bits.
    request.identifier = static_cast<std::uint16_t>(getpid());
    request.sequence = 1;

    fmt::println(
            "PING {} ({}) {}({}) bytes of data.",
            destname, dest, DATA_SIZE, IP_SIZE);
    first = last = std::chrono::steady_clock::now();
    while (true) {
        /* Start timer. */
        auto start = std::chrono::steady_clock::now();

        /* Send. */
        auto sendbytes = socket.send_to(request, dest);
        ++nsent;

        /* Receive. */
        address_v4 src;
        std::byte buffer[1024] = {};
        auto response = socket.receive_from<ip::header>(buffer, src);
        auto recvbytes = response.size();

        ++nrecv;

        /* Verify. */
        assert(response->message<icmp::echo>().type == icmp::message_type_t::ECHO_REPLY);
        assert(response->message<icmp::echo>().code == 0);
        assert(response->message<icmp::echo>().sequence == request.sequence);

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
                "{} bytes from {} ({}): icmp_seq={} ttl={} time={:.2f} ms",
                sendbytes, src.name(), src, request.sequence, response->ttl, ms);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        request.sequence += 1;
    }

    return EXIT_SUCCESS;
}
