#include <future>
#include <semaphore>
#include <vector>
#include <chrono>
#include <iostream>


void selfplay(int task_id);


int main(int argc, char* argv[])
{
    int tasks       = (argc >= 2) ? std::stoi(argv[1]) : 100;
    int concurrency = (argc >= 3) ? std::stoi(argv[2]) : 8;

    std::counting_semaphore<256> sem(concurrency);

    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<std::future<void>> jobs;

    for (int i = 0; i < tasks; ++i) {
        // sem.acquire();
        // try {
        //     selfplay(i + 1);
        // } catch (...) {
        //     sem.release();
        //     throw;
        // }
        // sem.release();
        sem.acquire();
        jobs.emplace_back(std::async(std::launch::async, [&, i] {
            selfplay(i + 1);
            sem.release();
        }));
    }
    for (auto& f : jobs) f.get();

    auto dt = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t0);
    std::cout << "\nFinished " << tasks << " games in " << dt.count() << "s.\n";
    return 0;
}
