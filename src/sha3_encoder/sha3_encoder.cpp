#include "pwgen/sha3_encoder/sha3_encoder.h"

SHA3Encoder::SHA3Encoder(const std::string &input_string)
    : encoder_(new CryptoPP::StringSink(output_string_))
    , input_string_(input_string)
{

    hash_.CalculateDigest(digest_, reinterpret_cast<const CryptoPP::byte *>(input_string_.c_str()), input_string_.length());

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

std::string SHA3Encoder::GetHexString() const
{
    return output_string_;
}

std::vector<uint8_t> SHA3Encoder::GetDigest() const
{
    return output_bytes_;
}

std::bitset<256> SHA3Encoder::GetBitset() const
{
    return output_bitset_;
}
