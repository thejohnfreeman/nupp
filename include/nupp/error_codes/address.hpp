#ifndef NUPP_ERROR_CODES_ADDRESS_HPP
#define NUPP_ERROR_CODES_ADDRESS_HPP

#include <nupp/export.hpp>

#include <system_error>

namespace nupp {
namespace error_codes {

// We need an enum for `std::is_error_code_enum`,
// which lets us return `std::error_code`.
enum class NUPP_EXPORT address_err {
    ADDRFAMILY = EAI_ADDRFAMILY,
    AGAIN = EAI_AGAIN,
    BADFLAGS = EAI_BADFLAGS,
    FAIL = EAI_FAIL,
    FAMILY = EAI_FAMILY,
    MEMORY = EAI_MEMORY,
    NODATA = EAI_NODATA,
    NONAME = EAI_NONAME,
    SERVICE = EAI_SERVICE,
    SOCKTYPE = EAI_SOCKTYPE,
    SYSTEM = EAI_SYSTEM,
};

namespace std {
    template <>
    struct is_error_code_enum<nupp::error_codes::address_err> : true_type {};
}

}
}

#endif
