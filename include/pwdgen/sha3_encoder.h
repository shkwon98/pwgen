#pragma once

#include <bitset>
#include <string>
#include <vector>

#include <cryptopp/hex.h>
#include <cryptopp/sha3.h>

class SHA3Encoder
{
public:
    SHA3Encoder(const std::string &input_string)
        : encoder_(new CryptoPP::StringSink(output_string_))
        , input_string_(input_string)
    {

        hash_.CalculateDigest(digest_, reinterpret_cast<const CryptoPP::byte *>(input_string_.c_str()),
                              input_string_.length());

        output_bytes_.resize(CryptoPP::SHA3_256::DIGESTSIZE);
        output_bytes_.assign(digest_, digest_ + CryptoPP::SHA3_256::DIGESTSIZE);

        output_bitset_ = std::bitset<256>();
        for (auto i = 0U; i < output_bytes_.size(); ++i)
        {
            auto bits = std::bitset<8>(output_bytes_[i]);
            for (auto j = 0U; j < bits.size(); ++j)
            {
                output_bitset_.set(i * 8 + j, bits.test(j));
            }
        }

        encoder_.Put(digest_, sizeof(digest_));
        encoder_.MessageEnd();
    }

    std::string GetHexString() const
    {
        return output_string_;
    }

    std::vector<uint8_t> GetDigest() const
    {
        return output_bytes_;
    }

    std::bitset<256> GetBitset() const
    {
        return output_bitset_;
    }

private:
    CryptoPP::SHA3_256 hash_;
    CryptoPP::byte digest_[CryptoPP::SHA3_256::DIGESTSIZE];
    CryptoPP::HexEncoder encoder_;

    std::string input_string_;
    std::string output_string_;
    std::vector<uint8_t> output_bytes_;
    std::bitset<256> output_bitset_;
};
