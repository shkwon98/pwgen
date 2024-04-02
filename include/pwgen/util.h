#pragma once

// standard headers
#include <bitset>
#include <iomanip>
#include <sstream>
#include <string>

namespace pwgen
{

std::array<int, 48> kPrimeNumberTable = { 2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,  53,
                                          59,  61,  67,  71,  73,  79,  83,  89,  97,  101, 103, 107, 109, 113, 127, 131,
                                          137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223 };

const std::string RowString(const uint32_t &mac, const std::string &key, const std::string &hash,
                            const std::string &password)
{
    std::stringstream ss;

    ss << std::setfill('0') << std::setw(6) << std::hex << std::uppercase << mac << key << ", " << hash << ", " << password;

    return ss.str();
}

template <size_t N>
std::bitset<N> ExtractBits(const std::bitset<256> &sha256_bitset)
{
    auto bitset = std::bitset<N>();
    for (auto i = 0U; i < N; ++i)
    {
        bitset.set(i, sha256_bitset.test(kPrimeNumberTable[i]));
    }

    return bitset;
}

} // namespace pwgen