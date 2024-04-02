#pragma once

// standard headers
#include <bitset>
#include <iomanip>
#include <sstream>
#include <string>

namespace pwgen
{

const std::string RowString(const uint32_t &mac, const std::string &key, const std::string &hash,
                            const std::string &password)
{
    std::stringstream ss;

    ss << std::setfill('0') << std::setw(6) << std::hex << std::uppercase << mac << key << ", " << hash << ", " << password;

    return ss.str();
}

template <size_t N>
std::string ToBase64(std::bitset<N> &value)
{
    std::string result;

    for (auto i = 0U; i < N; i += 6)
    {
        auto index = 0;
        for (auto j = 0; j < 6; ++j)
        {
            index <<= 1;
            index |= value[i + j];
        }

        result.push_back(kBase64Table[index]);
    }

    return result;
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