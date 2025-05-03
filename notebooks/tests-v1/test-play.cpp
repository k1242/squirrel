#include <filesystem>
#include <fstream>
#include <random>
#include <iomanip>
#include <cassert>
#include <span>
#include <chrono>
// #include <format>
#include <future>
#include <semaphore>
#include <mutex>
#include <cmath>
#include "Board.hpp"
#include "Squirrel.hpp"
#include "Tree.hpp"
#include "types.hpp"

struct GameHeader {
    I8  result;       //  in white perspective
    U16 halfmoves;    //  number of halfmoves
    U8  version;
};
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

void selfplay(int task_id)
{
    std::filesystem::create_directories("data/games");

    I8 result;
    Board    root_pos;
    Squirrel net("data/weights");
    Tree     tree(net);
    tree.new_game(root_pos);

    std::vector<RecordRaw> game;
    int ply = 0;

    while (true) {
        ++ply;

        const auto& root = tree.nodes[tree.root_idx];
        if (root.terminal) {
            result = std::round((2 * tree.sqr.root.side_to_move - 1) * root.leaf_eval);
            break;
        }

        for (int i = 0; i < 1000; ++i) tree.playout();

        assert(!root.moves.empty());

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

        int child_idx = tree.best_child(ply < 30);
        assert(child_idx != -1);

        rec.move = root.moves[child_idx].data;
        game.push_back(std::move(rec));

        tree.step(child_idx);
    }

    std::string rnd_name = random_name12();
    std::ofstream fout("data/games/" + rnd_name + ".bin",
                       std::ios::binary);
    GameHeader hdr{result, static_cast<uint16_t>(game.size()), 1};
    fout.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    for (const auto& rec : game) {
        fout.write(reinterpret_cast<const char*>(&rec.move),  sizeof(rec.move));
        fout.write(reinterpret_cast<const char*>(&rec.value), sizeof(rec.value));
        fout.put(static_cast<char>(rec.k));
        fout.write(reinterpret_cast<const char*>(rec.items.data()),
                   rec.items.size() * sizeof(PolicyItemBin));
    }

    std::string output = std::to_string(task_id) + ": " + rnd_name + " " + std::to_string(ply) + " "+ std::to_string(result) + "\n";
    std::cout << output;
}


int main(int argc, char* argv[])
{
    int tasks = 10;
    int concurrency = 8;

    if (argc >= 2) tasks = std::stoi(argv[1]);
    if (argc >= 3) concurrency = std::stoi(argv[2]);
    if (concurrency <= 0 || concurrency > 256) {
        std::cerr << "Invalid concurrency: " << concurrency << "\n";
        return 1;
    }

    std::counting_semaphore<256> sem(concurrency);
    std::mutex cout_mtx;

    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<std::future<void>> jobs;
    for (int i = 0; i < tasks; ++i) {
        sem.acquire();
        jobs.emplace_back(std::async(std::launch::async, [&, i] {
            try {
                selfplay(i+1);
                std::lock_guard lock(cout_mtx);
                // std::cout << "Done " << i + 1 << "\n";
            } catch (...) {
                std::lock_guard lock(cout_mtx);
                // std::cout << "Error in " << i + 1 << "\n";
            }
            sem.release();
        }));
    }

    for (auto& f : jobs) f.get();

    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dt = t1 - t0;
    std::cout << "\nFinished " << tasks << " games in " << dt.count() << "s.\n";
    return 0;
}
