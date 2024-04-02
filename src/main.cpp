// standard headers
#include <bitset>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

// Crypto++ headers
#include <cryptopp/base64.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>

#include "pwdgen/sha3_encoder.h"

std::array<char, 64> constexpr PasswordTable = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N',
                                                 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
                                                 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
                                                 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '2', '3', '4',
                                                 '5', '6', '7', '8', '9', '+', '-', '%', '$', '*', '<', '>' };

std::vector<int> primeNumbers = { 2,   3,   5,   7,   11,  13,  17,  19,  23,  29,  31,  37,  41,  43,  47,  53,
                                  59,  61,  67,  71,  73,  79,  83,  89,  97,  101, 103, 107, 109, 113, 127, 131,
                                  137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223 };

// Get hex string and convert it into bit vector
std::vector<bool> HexToBits(const std::string &hex_string)
{
    auto bit_vector = std::vector<bool>();
    bit_vector.reserve(hex_string.size() * 4);

    for (auto i = 0U; i < hex_string.size(); i += 2)
    {
        auto byte = std::stoi(hex_string.substr(i, 2), nullptr, 16);
        auto byte_bits = std::bitset<8>(byte);

        for (auto j = 0U; j < byte_bits.size(); ++j)
        {
            bit_vector.push_back(byte_bits.test(j));
        }
    }

    return bit_vector;
}

template <typename T>
std::string IntToHexString(T i)
{
    std::stringstream stream;
    stream << std::hex << i;
    return stream.str();
}

const std::string RowString(const uint32_t &mac, const std::string &key, const std::string &hash,
                            const std::string &password)
{
    std::stringstream ss;

    ss << std::setfill('0') << std::setw(6) << IntToHexString(mac) << key << ", " << hash << ", " << password;

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

        result.push_back(PasswordTable[index]);
    }

    return result;
}

template <size_t N>
std::bitset<N> ExtractBits(const std::bitset<256> &sha256_bitset)
{
    auto bitset = std::bitset<N>();
    for (auto i = 0U; i < N; ++i)
    {
        bitset.set(i, sha256_bitset.test(primeNumbers[i]));
    }

    return bitset;
}

std::stringstream CalculateHashes(uint32_t start, uint32_t end, const std::string &key, std::vector<std::string> &passwords)
{
    std::stringstream ss;

    for (auto i = start; i <= end; ++i)
    {
        const std::string mac_with_key = std::to_string(i) + key;

        SHA3Encoder hash_encoder(mac_with_key);

        const auto &digest = hash_encoder.GetDigest();
        const auto &hash_string = hash_encoder.GetHexString();
        const auto &hash_bitset = hash_encoder.GetBitset();

        auto password_bitset = ExtractBits<48>(hash_bitset);
        const auto &password = ToBase64(password_bitset);

        passwords.push_back(password);

        ss << RowString(i, key, hash_string, password) << std::endl;
    }

    return ss;
}

int main()
{
    auto thread_no = std::thread::hardware_concurrency();

    const auto &max_mac = 0xFFFFFFU;
    const auto &range = max_mac / thread_no;
    auto left_over = max_mac % thread_no;

    auto start_mac = 0U;
    auto end_mac = range;

    auto futures = std::vector<std::future<std::stringstream>>();
    auto threads = std::vector<std::thread>();
    auto password_lists = std::vector<std::vector<std::string>>(thread_no);

    auto key = std::string{ getpass("Enter a key: ") };

    for (auto i = 0U; i < thread_no; ++i)
    {
        auto task = std::packaged_task<std::stringstream(uint32_t, uint32_t, std::string &, std::vector<std::string> &)>(
            CalculateHashes);

        futures.emplace_back(task.get_future());
        password_lists[i].reserve(end_mac - start_mac + 1);
        threads.emplace_back(std::thread(std::move(task), start_mac, end_mac, std::ref(key), std::ref(password_lists[i])));

        start_mac = end_mac + 1;
        end_mac += range;

        if (i == thread_no - 2)
        {
            end_mac += left_over;
        }
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    std::set<std::string> password_set;
    uint32_t duplicate_count = 0;
    for (const auto &passwords : password_lists)
    {
        for (const auto &password : passwords)
        {
            if (!password_set.insert(password).second)
            {
                ++duplicate_count;
            }
        }
    }

    std::cout << "Duplicate password count: " << duplicate_count << std::endl;

    // 결과 파일에 출력
    auto file = std::ofstream("hashes.txt");

    file << "MAC + key, Hash, Password" << std::endl;
    for (auto &future : futures)
    {
        file << future.get().str();
    }
    file.close();

    return 0;
}
