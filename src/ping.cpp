#include <nupp/bytes_view.hpp>
#include <nupp/exceptions/address.hpp>
#include <nupp/exceptions/icmp.hpp>

#include <fmt/base.h>

#include <signal.h>     // sigaction
#include <unistd.h>     // close
#include <err.h>        // errno
#include <errno.h>      // errno
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <mutex>
#include <condition_variable>

#include <cassert>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <limits>

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
    fmt::println("{} packets transmitted, {} received, {:.1f}% packet loss, time ????ms",
            nsent, nrecv, static_cast<double>(nsent - nrecv) / nsent);
    double mean   = total / nrecv;
    double stddev = std::sqrt((sos / nrecv) - (mean * mean));
    fmt::println("rtt min/avg/max/mdev = {:.3f}/{:.3f}/{:.3f}/{:.3f} ms",
            min, mean, max, stddev);
    std::fflush(nullptr);
}

void onsignal(int signal) {
    if (SIGINT == signal) exit(EXIT_SUCCESS);
}

void ping(
    socket_v4& socket, icmp::echo& message, address_v4 const& dest)
{
  /* Prepare to receive. We want minimal processing between send and
   // * receive. */
  // struct icmp* req = (struct icmp*)sock.buffer;
// #define MAX_PACKET_SIZE 64
  // u8_t buffer[MAX_PACKET_SIZE] = { 0 };
  // struct sockaddr_in src;

  // /* Start timer. */
  // struct timeval start, finish;
  // gettimeofday(&start, /*timezone=*/NULL);

  // /* Send. */
  // icmp::set_checksum(message);
  // auto sendbytes = socket.send_to(message, dest);
  // ++nsent;

  // /* Print. */
  // if (message.sequence() == 0) {
   //  fmt::print("PING {} ({}): {} data bytes\n", destname, dest, sendbytes);
  // }

  // /* Receive. */
  // ssize_t recvbytes = jficmp_recv(&sock, buffer, MAX_PACKET_SIZE, &src);
  // ++nrecv;

  // /* Stop timer. */
  // gettimeofday(&finish, /*timezone=*/NULL);
  // float ms = (((finish.tv_sec - start.tv_sec) * 1000000.0) +
   //    (finish.tv_usec - start.tv_usec)) / 1000.0;
  // if (ms < min) min = ms;
  // if (ms > max) max = ms;
  // total   += ms;
  // sos += ms * ms;

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
}

#define PRINT(T) \
    fmt::println("{:16} {:4} {:4}", #T, sizeof(T), alignof(T))

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
  std::atexit(onexit);

  /* Construct ICMP header. */
  auto message = icmp::echo{};
  // Narrowing conversion from 32 to 16 bits.
  message.identifier = static_cast<std::uint16_t>(getpid());
  fmt::println("{}", message.identifier);
  message.sequence = 1;
  fmt::println("{}", nupp::to_bytes(message));

  // while (1) {
  //   ping(socket, message, dest);
  //   sleep(1);
  //   message.sequence += 1;
  // }

  return EXIT_SUCCESS;
}
