#include <nupp/bytes_view.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/icmp.hpp>

#include <fmt/base.h>

#include <unistd.h>     // close
#include <err.h>        // errno
#include <errno.h>      // errno
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

  /* Prepare to receive. We want minimal processing between send and
   // * receive. */
  // struct icmp* req = (struct icmp*)sock.buffer;
// #define MAX_PACKET_SIZE 64
  // u8_t buffer[MAX_PACKET_SIZE] = { 0 };
  // struct sockaddr_in src;

  // /* Receive. */
  // ssize_t recvbytes = jficmp_recv(&sock, buffer, MAX_PACKET_SIZE, &src);
  // ++nrecv;

  // /* Verify. */
  // struct ip* resp_ip;
  // struct icmp* resp_icmp;
  // jficmp_open(buffer, &resp_ip, &resp_icmp);
  // assert(resp_icmp->icmp_type == ICMP_ECHOREPLY);
  // assert(resp_icmp->icmp_code == 0);
  // assert(resp_icmp->icmp_id   == req->icmp_id);
  // assert(resp_icmp->icmp_seq  == req->icmp_seq);

  // /* Print. */
  // printf("%ld bytes from %d.%d.%d.%d: icmp_seq=%d ttl=%d time=%.3f ms\n",
   //    recvbytes,
   //    buffer[12], buffer[13], buffer[14], buffer[15],
   //    ntohs(resp_icmp->icmp_seq), resp_ip->ip_ttl, ms);

#define PRINT(T) \
    fmt::println("{:16} {:4} {:4}", #T, sizeof(T), alignof(T))

#define EXPR(expr) fmt::println("{} = {}", #expr, expr)

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

    // std::byte buffer[1024] = {std::byte{}};
    // auto message = nupp::message<icmp::echo_header>::construct(buffer, 8);
    // auto& body = *message;

    icmp::echo<0> message;
    auto& body = message;

    // Narrowing conversion from 32 to 16 bits.
    body.identifier = static_cast<std::uint16_t>(getpid());
    body.sequence = 1;

    fmt::println("PING {} ({}) xx(xx) bytes of data.", destname, dest);
    while (1) {
        /* Start timer. */
        auto start = std::chrono::steady_clock::now();

        /* Send. */
        auto sendbytes = socket.send_to(message, dest);
        ++nsent;

        /* Receive. */
        // auto recvbytes = socket.recv_from(message, dest);
        ++nrecv;

        /* Stop timer. */
        auto stop = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> d = stop - start;
        auto ms = d.count();
        if (ms < min) min = ms;
        if (ms > max) max = ms;
        total += ms;
        sos += ms * ms;

        fmt::println("{} bytes from xx.net (xx): icmp_seq={} ttl=xx time={:.2f} ms",
                sendbytes, body.sequence, ms);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        break;
        // body.sequence += 1;
    }

    return EXIT_SUCCESS;
}
