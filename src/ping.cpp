#include <nupp/bytes_view.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/icmp.hpp>

#include <fmt/base.h>

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

void onexit() {
    fmt::println("\n--- {} ping statistics ---", destname);
    fmt::println("{} packets transmitted, {} received, {:.0f}% packet loss, time xxxxms",
            nsent, nrecv, 100.0 * (nsent - nrecv) / nsent);
    double mean   = total / nrecv;
    double stddev = std::sqrt((sos / nrecv) - (mean * mean));
    fmt::println("rtt min/avg/max/mdev = {:.3f}/{:.3f}/{:.3f}/{:.3f} ms",
            min, mean, max, stddev);
    std::fflush(nullptr);
}

void onsignal(int signal) {
    if (SIGINT == signal) exit(EXIT_SUCCESS);
}

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
    constexpr std::size_t TOTAL_SIZE = 84;
    constexpr std::size_t ICMP_SIZE = TOTAL_SIZE - 20;
    constexpr std::size_t DATA_SIZE = ICMP_SIZE - 8;

    // std::byte buffer[1024] = {std::byte{}};
    // auto outgoing = nupp::message<icmp::echo_header>::construct(buffer, ICMP_SIZE);
    // auto& body = *outgoing;

    icmp::echo<DATA_SIZE> outgoing;
    auto& body = outgoing;

    // Narrowing conversion from 32 to 16 bits.
    body.identifier = static_cast<std::uint16_t>(getpid());
    body.sequence = 1;

    std::byte obuffer[1024] = {std::byte{}};
    address_v4 src;

    fmt::println(
            "PING {} ({}) {}({}) bytes of data.",
            destname, dest, DATA_SIZE, TOTAL_SIZE);
    while (1) {
        /* Start timer. */
        auto start = std::chrono::steady_clock::now();

        /* Send. */
        auto sendbytes = socket.send_to(outgoing, dest);
        ++nsent;

        /* Receive. */
        auto incoming = socket.receive_from<icmp::echo_header>(nupp::wbytes<>{obuffer, 1024}, src);
        auto recvbytes = incoming.size();
        ++nrecv;

        /* Verify. */
        assert(incoming->type == ICMP_ECHOREPLY);
        assert(incoming->code == 0);
        assert(incoming->sequence == body.sequence);

        /* Stop timer. */
        auto stop = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> d = stop - start;
        auto ms = d.count();
        if (ms < min) min = ms;
        if (ms > max) max = ms;
        total += ms;
        sos += ms * ms;

        /* Print. */
        fmt::println(
                "{} bytes from xx.net ({}): icmp_seq={} ttl=xx time={:.2f} ms",
                sendbytes, src, body.sequence, ms);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        body.sequence += 1;
    }

    return EXIT_SUCCESS;
}
