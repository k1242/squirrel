#include <filesystem>
#include <fstream>
#include <random>
#include <iomanip>
#include <cassert>
#include <span>
#include <chrono>
#include <format>
#include <future>
#include <semaphore>
#include <mutex>
#include "Board.hpp"
#include "Squirrel.hpp"
#include "Tree.hpp"
#include "types.hpp"

constexpr int TASKS            = 10;
constexpr int MAX_CONCURRENCY  = 8;

struct PolicyItemBin {
    U16 move;
    U32 visits;
};
struct RecordRaw {
    U16 move;
    float value;
    U8  k;
    std::vector<PolicyItemBin> items;
};

// thread-local 12-digit ID
static std::string random_name12()
{
    static thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<uint64_t> d(0, 999'999'999'999ULL);
    std::ostringstream s;
    s << std::setw(12) << std::setfill('0') << d(rng);
    return s.str();
}

void selfplay()
{
    std::filesystem::create_directories("data/bin");

    Board    root_pos;
    Squirrel net("weights");
    Tree     tree(net);
    tree.new_game(root_pos);

    std::vector<RecordRaw> game;
    int ply = 0;

    while (true) {
        ++ply;
        for (int i = 0; i < 2000; ++i) tree.playout();

        const auto& root = tree.nodes[tree.root_idx];
        if (root.moves.empty()) break;

        float v = root.visits ? root.total_value / root.visits : 0.0f;

        RecordRaw rec;
        rec.value = v;
        rec.k     = static_cast<U8>(root.moves.size());        // safe cast
        rec.items.reserve(rec.k);

        for (size_t i = 0; i < root.moves.size(); ++i) {
            int cidx = root.children[i];
            U32 vis  = (cidx != -1) ? tree.nodes[cidx].visits : 0;
            rec.items.push_back({ root.moves[i].data, vis });
        }

        int child_idx = tree.best_child(ply > 30);
        assert(child_idx != -1);

        rec.move = root.moves[child_idx].data;
        game.push_back(std::move(rec));

        tree.step(child_idx);
    }

    std::ofstream fout("data/bin/" + random_name12() + ".bin",
                       std::ios::binary);
    for (const auto& rec : game) {
        fout.write(reinterpret_cast<const char*>(&rec.move),  sizeof(rec.move));
        fout.write(reinterpret_cast<const char*>(&rec.value), sizeof(rec.value));
        fout.put(static_cast<char>(rec.k));
        fout.write(reinterpret_cast<const char*>(rec.items.data()),
                   rec.items.size() * sizeof(PolicyItemBin));
    }
}


std::counting_semaphore<MAX_CONCURRENCY> sem(MAX_CONCURRENCY);
std::mutex cout_mtx;

void selfplay_task(int id)
{
    sem.acquire();
    try {
        selfplay();
        std::lock_guard lock(cout_mtx);
        std::cout << std::format("Done {:02d}\n", id);
    } catch (...) {
        std::lock_guard lock(cout_mtx);
        std::cerr << std::format("Error in task {:02d}\n", id);
    }
    sem.release();
}

int main()
{
    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<std::future<void>> jobs;
    for (int i = 0; i < TASKS; ++i)
        jobs.emplace_back(std::async(std::launch::async, selfplay_task, i + 1));

    for (auto& f : jobs) f.get();

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt = t1 - t0;
    std::cout << std::format("Finished {} games in {:.3f}s.\n", TASKS, dt.count());
    return 0;
}
