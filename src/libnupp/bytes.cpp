#include <nupp/bytes.hpp>

#include <cstdint>

namespace nupp {
namespace detail {

static char const* const HEADER = "     |     0     |     1     |     2     |     3     |\n";
static char const* const HLINE  = "     +-----------------------------------------------+\n";
static char const* const PREFIX = "     ";
static char const* const POSTFIX = "|\n";

static char const* const NIBBLES[] = {
    "0000 ",
    "0001 ",
    "0010 ",
    "0011 ",

    "0100 ",
    "0101 ",
    "0110 ",
    "0111 ",

    "1000 ",
    "1001 ",
    "1010 ",
    "1011 ",

    "1100 ",
    "1101 ",
    "1110 ",
    "1111 ",
};

void pretty_printing::intro() const {
    auto addr = reinterpret_cast<std::uintptr_t>(bytes.data());
    out << fmt::format("0x{:x}....\n", addr >> 16);
    out << HEADER;
}

void pretty_printing::line() const {
    // TODO: Parameterize NCOLS.
    constexpr std::size_t const NCOLS = 4;
    auto it = bytes.begin() + index;
    // Prefix row with last 4 hexdigits of address.
    auto addr = reinterpret_cast<std::uintptr_t>(&*it);
    fmt::print(out, "{:04x} ", addr & 0xFFFF);
    auto sep = "| ";
    for (int j = 0; j < NCOLS; ++j) {
        out << sep;
        if (it == bytes.end()) {
            // 10 spaces.
            out << "          ";
            sep = "  ";
        } else {
            auto b = static_cast<std::uint8_t>(*it);
            out << NIBBLES[b >> 4] << NIBBLES[b & 0xF];
            ++it;
        }
    }
    out << "|\n";
}

void pretty_printing::print(char* buffer, std::size_t nbytes) const {
    // TODO: replace with better exceptions
    assert(4 - (index % 4) >= nbytes);
    assert(bytes.size() - index >= nbytes);
    if (index % 4 == 0) {
        out << HLINE;
        line();
        out << PREFIX;
    }
    out << buffer;
    index += nbytes;
    if (index % 4 == 0)
        out << POSTFIX;
}

std::ostream& pretty_printing::outro() const {
    if (index % 4 != 0) {
        // Finish line-in-progress.
        auto remainder = 4 - (index % 4);
        fmt::print(out, "|{:{}}", " ", remainder * 12 - 1);
        out << POSTFIX;
        index += remainder;
    }
    auto row = 0;
    while (index < bytes.size()) {
        if (row % 2 == 0) out << HLINE;
        ++row;
        line();
        index += 4;
    }
    out << HLINE;
    return out;
}

}
}

namespace std {

std::ostream& operator<< (std::ostream& out, nupp::rbytes<> const& bytes) {
    return out << nupp::pretty_printer(bytes) << nupp::end{};
}

}
