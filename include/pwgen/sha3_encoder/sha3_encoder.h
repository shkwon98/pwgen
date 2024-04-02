#pragma once

// standard headers
#include <bitset>
#include <string>
#include <vector>

// Crypto++ headers
#include <cryptopp/hex.h>
#include <cryptopp/sha3.h>

class SHA3Encoder
{
public:
    SHA3Encoder(const std::string &input_string);

    std::string GetHexString() const;
    std::vector<uint8_t> GetDigest() const;
    std::bitset<256> GetBitset() const;

private:
    CryptoPP::SHA3_256 hash_;
    CryptoPP::byte digest_[CryptoPP::SHA3_256::DIGESTSIZE];
    CryptoPP::HexEncoder encoder_;

    std::string input_string_;
    std::string output_string_;
    std::vector<uint8_t> output_bytes_;
    std::bitset<256> output_bitset_;
};
