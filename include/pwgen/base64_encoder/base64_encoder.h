#pragma once

// standard headers
#include <array>
#include <bitset>
#include <string>
#include <vector>

namespace pwgen
{

class Base64Encoder
{
public:
    template <size_t N>
    Base64Encoder(const std::bitset<N> &value)
        : output_string_()
    {
        for (auto i = 0U; i < N; i += 6)
        {
            auto index = 0;
            for (auto j = 0; j < 6; ++j)
            {
                index <<= 1;
                index |= value[i + j];
            }

            output_string_.push_back(base64EncodingTable_[index]);
        }
    }

    std::string GetEncodedBase64() const
    {
        return output_string_;
    }

private:
    std::string output_string_;
    const std::array<char, 64> base64EncodingTable_ = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N',
                                                        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
                                                        'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
                                                        'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '2', '3', '4',
                                                        '5', '6', '7', '8', '9', '+', '-', '%', '$', '*', '<', '>' };
};

} // namespace pwgen