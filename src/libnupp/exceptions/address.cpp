#include <nupp/exceptions/address.hpp>

#include <netdb.h>

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

address_v4 resolve_v4(char const* hostname) {
    address_v4 addr;
    struct addrinfo* res = NULL;
    int error = getaddrinfo(hostname, /*serv=*/NULL, /*hints=*/NULL, &res);
    if (error) {
        throw std::system_error(error, the_address_error_category);
    }
    assert(res);
    std::memcpy(&addr, res->ai_addr, sizeof(addr));
    return addr;
}

}
}
