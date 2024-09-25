#include <nupp/bytes_view.hpp>

#include <cstdint>

static char const* const HEADER = "     |     0     |     1     |     2     |     3     |\n";
static char const* const HLINE  = "     +-----------------------------------------------+\n";

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

namespace std {

std::ostream& operator<< (std::ostream& out, bytes_view const bv) {
    // TODO: Parameterize ncols, left shift
    constexpr std::size_t const NCOLS = 4;
    auto addr = reinterpret_cast<std::uintptr_t>(bv.data());
    out << fmt::format("0x{:x}....\n", addr >> 16);
    out << HEADER;
    int row = 0;
    auto it = bv.begin();
    while (it != bv.end()) {
        if (row % 2 == 0) out << HLINE;
        ++row;
        // Prefix row with last 4 hexdigits of address.
        auto addr = reinterpret_cast<std::uintptr_t>(&*it);
        out << fmt::format("{:x} ", addr & 0xFFFF);
        for (int j = 0; j < NCOLS; ++j) {
            out << "| ";
            if (it == bv.end()) {
                // 10 spaces.
                out << "          ";
            } else {
                auto b = static_cast<unsigned char>(*it);
                out << NIBBLES[b >> 4] << NIBBLES[b & 0xF];
                ++it;
            }
        }
        out << "|\n";
    }
    out << HLINE;
    return out;
}

}
