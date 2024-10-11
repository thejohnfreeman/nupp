#include <nupp/exceptions/address.hpp>

#include <fmt/format.h>

#include <arpa/inet.h> // inet_pton
#include <netdb.h>
#include <netinet/in.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>
#include <system_error>

namespace nupp {
namespace exceptions {

struct address_error_category : public std::error_category {
    char const* name() const noexcept override {
        return "address";
    }

    std::string message(int ev) const {
        return gai_strerror(ev);
    }
};

address_error_category const the_address_error_category;

address_v4::address_v4() {
  std::memset(this, 0, sizeof(*this));
  this->sin_family = AF_INET;
}

address_v4& address_v4::operator= (sockaddr const& rhs) {
    std::memcpy(this, &rhs, sizeof(sockaddr_in));
    return *this;
}

std::string address_v4::name() const {
    std::string name(32, '\0');
    int errc = ::getnameinfo(
            *this, sizeof(*this),
            name.data(), name.size(),
            /*serv=*/nullptr, /*servlen=*/0,
            /*flags=*/0);
    if (errc == EAI_OVERFLOW) {
        // retry with larger buffer.
    } else if (errc) {
        throw std::system_error(errc, the_address_error_category);
    }
    return name;
}

address_v4 address_v4::from(std::uint32_t addr32) {
    address_v4 address;
    address.sin_addr.s_addr = addr32;
    return address;
}

address_v4 address_v4::of(char const* hostname) {
    addrinfo* result = nullptr;
    if (int errc = ::getaddrinfo(hostname, /*serv=*/nullptr, /*hints=*/nullptr, &result)) {
        throw std::system_error(errc, the_address_error_category);
    }
    assert(result);
    address_v4 address;
    address = *result->ai_addr;
    return address;
}

address_v4 address_v4::parse(char const* notation) {
    address_v4 address;
    if (int errc = ::inet_pton(AF_INET, notation, &address); !errc) {
        throw std::runtime_error("not an address");
    }
    return address;
}

address_v4 address_v4::any() {
    return from(INADDR_ANY);
}

}
}
