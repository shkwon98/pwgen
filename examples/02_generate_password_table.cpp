// standard headers
#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#endif

// project headers
#include "pwgen/base64_encoder/base64_encoder.h"
#include "pwgen/sha3_hasher/sha3_hasher.h"
#include "pwgen/util.h"

static std::string GetSuffixKey()
{
#ifdef _WIN32
    std::cout << "Enter a suffix key: ";

    std::string password;
    int ch;
    while ((ch = _getch()) != '\r' && ch != '\n')
    {
        if (ch == '\b')
        { // Handle backspace
            if (!password.empty())
            {
                std::cout << "\b \b"; // Erase the character from console
                password.pop_back();
            }
        }
        else
        {
            password.push_back(static_cast<char>(ch));
            std::cout << "*"; // Print asterisk instead of actual character
        }
    }
    std::cout << std::endl;
    return password;
#else
    return getpass("Enter a suffix key: ");
#endif
}

static std::stringstream GetPasswordTable(uint32_t start, uint32_t end, const std::string &key)
{
    std::stringstream ss;

    for (auto i = start; i <= end; ++i)
    {
        const auto &mac = pwgen::ToHexString(i);
        const auto &mac_with_key = mac + key;

        pwgen::SHA3Hasher hash_encoder(mac_with_key);

        const auto &hash_string = hash_encoder.GetHexString();
        const auto &hash_bitset = hash_encoder.GetBitset();

        auto password_bitset = pwgen::ExtractBits<48>(hash_bitset);

        pwgen::Base64Encoder password_encoder(password_bitset);
        const auto &password = password_encoder.GetEncodedBase64();

        ss << std::setfill('0') << std::setw(6) << std::hex << mac << ", " << key << ", " << hash_string << ", " << password
           << std::endl;
    }

    return ss;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <output_file>" << std::endl;
        return 1;
    }

    const auto &max_mac = 0xFFFFFFU;
    const auto &thread_no = std::thread::hardware_concurrency();
    const auto &range = max_mac / thread_no;
    const auto &left_over = max_mac % thread_no;

    auto futures = std::vector<std::future<std::stringstream>>();
    auto threads = std::vector<std::thread>();

    auto suffix = GetSuffixKey();

    auto start_mac = 0U;
    auto end_mac = range;

    for (auto i = 0U; i < thread_no; ++i)
    {
        auto task = std::packaged_task<std::stringstream(uint32_t, uint32_t, const std::string &)>(GetPasswordTable);
        futures.emplace_back(task.get_future());
        threads.emplace_back(std::thread(std::move(task), start_mac, end_mac, std::ref(suffix)));

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

    auto file = std::ofstream(argv[1]);
    file << "MAC, key, Hash, Password" << std::endl;
    for (auto &future : futures)
    {
        file << future.get().str();
    }

    file.close();
    return 0;
}
