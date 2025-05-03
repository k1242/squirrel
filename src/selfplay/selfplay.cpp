#include "format.hpp"
#include "Board.hpp"
#include "Squirrel.hpp"
#include "Tree.hpp"
#include "types.hpp"

#include <filesystem>
#include <fstream>
#include <random>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <vector>
#include <chrono>
#include <iostream>
#include <string>

namespace {
// 5‑digit thread‑local random id
std::string random_name()
{
    static thread_local std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<uint64_t> d(0, 99999ULL);
    std::ostringstream s;
    s << std::setw(5) << std::setfill('0') << d(rng);
    return s.str();
}

// temporary record during generation
struct RecordRaw {
    U16 move;
    float value;
    U8  k;
    std::vector<PolicyItemBin> items;
};
}

void selfplay(int task_id)
{
    std::filesystem::create_directories("data/games");

    I8 result;
    Board root_pos;
    Squirrel net("data/weights");
    Tree tree(net);
    tree.new_game(root_pos);

    std::vector<RecordRaw> game;
    int ply = 0;

    while (true) {
        ++ply;
        const auto& root = tree.nodes[tree.root_idx];
        if (root.terminal) {
            result = std::round((2 * tree.sqr.root.side_to_move - 1) * root.leaf_eval); // white perspective
            break;
        }

        for (int i = 0; i < 1000; ++i) tree.playout();
        assert(!root.moves.empty());

        float v = root.visits ? root.total_value / root.visits : 0.0f;

        RecordRaw rec;
        rec.value = v;
        rec.k = static_cast<U8>(root.moves.size());
        rec.items.reserve(rec.k);

        for (size_t i = 0; i < root.moves.size(); ++i) {
            int cidx = root.children[i];
            U32 vis = cidx != -1 ? tree.nodes[cidx].visits : 0;
            rec.items.push_back({root.moves[i].data, vis});
        }

        int child_idx = tree.best_child(ply < 30);
        assert(child_idx != -1);

        rec.move = root.moves[child_idx].data;
        game.push_back(std::move(rec));

        tree.step(child_idx);
    }

    std::string name = random_name();
    std::ofstream fout("data/games/" + name + ".bin", std::ios::binary);

    GameHeader hdr{result, static_cast<uint16_t>(game.size()), 1};
    fout.write(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    for (const auto& rec : game) {
        fout.write(reinterpret_cast<const char*>(&rec.move),  sizeof(rec.move));
        fout.write(reinterpret_cast<const char*>(&rec.value), sizeof(rec.value));
        fout.put(static_cast<char>(rec.k));
        fout.write(reinterpret_cast<const char*>(rec.items.data()),
                   rec.items.size() * sizeof(PolicyItemBin));
    }

    std::string output = std::to_string(task_id) + " " + name + " " + std::to_string(ply) + " " + std::to_string(result) + "\n";

    std::cout << output;
}
