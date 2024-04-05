// standard headers
#include <future>
#include <iostream>
#include <set>
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

static std::vector<std::string> GeneratePasswords(uint32_t start, uint32_t end, const std::string &key)
{
    std::vector<std::string> passwords;

    for (auto i = start; i <= end; ++i)
    {
        const auto &mac = pwgen::ToHexString(i);
        const auto &mac_with_key = mac + key;

        pwgen::SHA3Hasher hash_encoder(mac_with_key);

        const auto &hash_bitset = hash_encoder.GetBitset();

        auto password_bitset = pwgen::ExtractBits<48>(hash_bitset);

        pwgen::Base64Encoder password_encoder(password_bitset);
        const auto &password = password_encoder.GetEncodedBase64();

        passwords.push_back(password);
    }

    return passwords;
}

int main(void)
{
    const auto &max_mac = 0xFFFFFFU;
    const auto &thread_no = std::thread::hardware_concurrency();
    const auto &range = max_mac / thread_no;
    const auto &left_over = max_mac % thread_no;

    auto password_lists = std::vector<std::vector<std::string>>(thread_no);
    auto futures = std::vector<std::future<std::vector<std::string>>>();
    auto threads = std::vector<std::thread>();

    auto suffix = GetSuffixKey();
    auto start_mac = 0U;
    auto end_mac = range;

    for (auto i = 0U; i < thread_no; ++i)
    {
        auto &password_list = password_lists[i];
        password_list.reserve(end_mac - start_mac + 1);

        auto task = std::packaged_task<std::vector<std::string>(uint32_t, uint32_t, const std::string &)>(GeneratePasswords);
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

    auto password_set = std::set<std::string>();
    auto duplication_counter = 0U;

    for (auto &future : futures)
    {
        const auto &passwords = future.get();

        for (const auto &password : passwords)
        {
            if (!password_set.insert(password).second)
            {
                ++duplication_counter;
            }
        }
    }

    std::cout << "Duplicate password count: " << duplication_counter << std::endl;
    return 0;
}
