// standard headers
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

// Crypto++ headers
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha3.h>

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

std::stringstream CalculateHashes(uint32_t start, uint32_t end, const std::string &key)
{
    std::stringstream ss;

    for (auto i = start; i <= end; ++i)
    {
        const std::string mac_with_key = std::to_string(i) + key;

        CryptoPP::SHA3_256 hash;
        CryptoPP::byte digest[CryptoPP::SHA3_256::DIGESTSIZE];

        hash.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte *>(mac_with_key.c_str()), mac_with_key.length());

        std::string output;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(output));
        encoder.Put(digest, sizeof(digest));
        encoder.MessageEnd();

        ss << RowString(i, key, output, "-") << std::endl;
    }

    return ss;
}

int main()
{
    auto thread_no = std::thread::hardware_concurrency();

    const auto &max_mac = 0x0FFFFFU;
    const auto &range = max_mac / thread_no;
    auto left_over = max_mac % thread_no;

    auto start_mac = 0U;
    auto end_mac = range;

    auto futures = std::vector<std::future<std::stringstream>>();
    auto threads = std::vector<std::thread>();

    auto key = std::string{ getpass("Enter a key: ") };

    for (auto i = 0U; i < thread_no; ++i)
    {
        auto task = std::packaged_task<std::stringstream(uint32_t, uint32_t, std::string &)>(CalculateHashes);

        futures.emplace_back(task.get_future());
        threads.emplace_back(std::thread(std::move(task), start_mac, end_mac, std::ref(key)));

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
