// standard headers
#include <iostream>
#include <string>
#include <vector>

// project headers
#include "pwgen/base64_encoder/base64_encoder.h"
#include "pwgen/sha3_hasher/sha3_hasher.h"
#include "pwgen/util.h"

std::string GetPassword(const std::string &key)
{
    pwgen::SHA3Hasher hash_encoder(key);

    const auto &hash_bitset = hash_encoder.GetBitset();
    auto password_bitset = pwgen::ExtractBits<48>(hash_bitset);

    pwgen::Base64Encoder password_encoder(password_bitset);

    return password_encoder.GetEncodedBase64();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <key>" << std::endl;
        return 1;
    }

    const auto &key = std::string(argv[1]);
    const auto &password = GetPassword(key);

    std::cout << password;

    return 0;
}
