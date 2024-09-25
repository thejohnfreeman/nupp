#include <nupp/exceptions/address.hpp>

#include <fmt/format.h>

#include <netdb.h>
#include <netinet/in.h>

#include <cassert>
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

address_v4 address_v4::from(std::uint32_t addr32) {
    address_v4 address;
    address.sin_addr.s_addr = addr32;
    return address;
}

address_v4 address_v4::any() {
    return from(INADDR_ANY);
}

address_v4 address_v4::of(char const* hostname) {
    struct addrinfo* res = NULL;
    int error = getaddrinfo(hostname, /*serv=*/NULL, /*hints=*/NULL, &res);
    if (error) {
        throw std::system_error(error, the_address_error_category);
    }
    assert(res);
    address_v4 address;
    address = *res->ai_addr;
    return address;
}

}
}
