// standard headers
#include <fstream>
#include <future>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

// project headers
#include "pwgen/common.h"
#include "pwgen/sha3_encoder/sha3_encoder.h"
#include "pwgen/util.h"

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

        auto password_bitset = pwgen::ExtractBits<48>(hash_bitset);
        const auto &password = pwgen::ToBase64(password_bitset);

        passwords.push_back(password);

        ss << pwgen::RowString(i, key, hash_string, password) << std::endl;
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

    auto file = std::ofstream("hashes.txt");

    file << "MAC + key, Hash, Password" << std::endl;
    for (auto &future : futures)
    {
        file << future.get().str();
    }
    file.close();

    return 0;
}
