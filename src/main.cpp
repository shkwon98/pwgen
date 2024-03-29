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

const std::string RowString(const std::string &mac_with_key, const std::string &hash, const std::string &password)
{
    std::stringstream ss;
    ss << mac_with_key << ", " << hash << ", " << password;
    return ss.str();
}

std::stringstream CalculateHashes(int start, int end, const std::string &key)
{
    std::stringstream ss;

    for (int i = start; i <= end; ++i)
    {
        const std::string mac_with_key = std::to_string(i) + key;

        CryptoPP::SHA3_256 hash;
        CryptoPP::byte digest[CryptoPP::SHA3_256::DIGESTSIZE];

        hash.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte *>(mac_with_key.c_str()), mac_with_key.length());

        std::string output;
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(output));
        encoder.Put(digest, sizeof(digest));
        encoder.MessageEnd();

        ss << RowString(mac_with_key, output, "-") << std::endl;
    }

    return ss;
}

int main()
{
    auto key = std::string{ getpass("Enter a key: ") };

    // SHA-3 해시 함수 초기화
    CryptoPP::SHA3_256 sha3;

    // 스레드 개수 설정
    auto thread_no = std::thread::hardware_concurrency();

    // 각 스레드에 할당할 작업 범위 계산
    auto range = 0x0FFFFF / thread_no;
    auto start = 0;
    auto end = range;

    // std::packaged_task container for each thread
    auto task = std::vector<std::packaged_task<std::stringstream(int, int, std::string &, CryptoPP::SHA3_256 &)>>();

    // 스레드 컨테이너 생성
    auto threads = std::vector<std::thread>();
    auto futures = std::vector<std::future<std::stringstream>>();

    // 스레드 생성 및 작업 할당
    for (auto i = 0U; i < thread_no; ++i)
    {
        // Create a packaged_task with the CalculateHashes function
        std::packaged_task<std::stringstream(int, int, std::string &, CryptoPP::SHA3_256 &)> task(CalculateHashes);

        // Get the future associated with the packaged_task
        futures.emplace_back(task.get_future());

        // Start the thread and pass the packaged_task as an argument
        threads.emplace_back(std::thread(std::move(task), start, end, std::ref(key)));

        // Update the start and end values for the next thread
        start = end + 1;
        end += range;
    }

    // 스레드 완료 대기
    for (auto &thread : threads)
    {
        thread.join();
    }

    // 결과 파일에 출력
    std::ofstream file("hashes.txt");
    file << "MAC + key, Hash, Password" << std::endl;
    for (auto &future : futures)
    {
        file << future.get().str();
    }
    file.close();

    return 0;
}
