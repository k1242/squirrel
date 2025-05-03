#include "Board.hpp"
#include "Squirrel.hpp"
#include "Tree.hpp"
#include "notation.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>

void print_root_moves(const Tree& tree) {
    const auto& root = tree.nodes[tree.root_idx];
    std::vector<int> indices(root.moves.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return tree.nodes[root.children[a]].visits > tree.nodes[root.children[b]].visits;
    });

    for (int i : indices) {
        std::cout << std::format("{}: {}/{}\n", root.moves[i].uci(), tree.nodes[root.children[i]].total_value, tree.nodes[root.children[i]].visits);
    }
}

int main()
{
    int child_idx;
    std::cout << std::format("Start.\n");
    // path to directory with *.npy model files
    // std::string weights_dir = ;

    // starting position
    // Board root_pos;
    // Board root_pos("r3kbnr/p4ppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Board root_pos("8/8/8/8/5k2/1q6/8/5K2 w - - 0 1");
    // Board root_pos("1nb5/Q4p1k/2B5/3Pq3/1Pp1p2r/P3P3/3K2R1/1N2N3 w - - 1 38");
    // Board root_pos("1nb5/5Q1k/2B5/3Pq3/1Pp1p2r/P3P3/3K2R1/1N2N3 b - - 1 39");
    // Board root_pos("r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5Q2/PPPP1PPP/RNB1K1NR b KQkq - 0 1");

    // neural evaluator
    Squirrel net("weights");

    // MCTS tree
    Tree tree(net);

    auto timer_start = std::chrono::high_resolution_clock::now();

    tree.new_game(root_pos);
    // print_board(tree.sqr.root);

    int ply = 0;
    while (true) {
        ply++;

        for (int i = 0; i < 20000; ++i) 
            // std::cout << std::format("--- {:06d} ---\n", i);
            tree.playout();

        // print_root_moves(tree);

        child_idx = tree.best_child(false);
        
        if (child_idx == -1) {
            // print_board(tree.sqr.root);
            // std::cout << std::format("Game over.\n");
            break;
        }

        tree.step(child_idx);

        // print_board(tree.sqr.root);
        // std::cout << std::format("{:03d}: {:.4f}\n", ply, (1-2*tree.sqr.root.side_to_move)*tree.nodes[tree.root_idx].total_value / tree.nodes[tree.root_idx].visits);
    }

    auto timer_end = std::chrono::high_resolution_clock::now();
    // float res =  (1-2*tree.sqr.root.side_to_move)*tree.nodes[tree.root_idx].leaf_eval;
    float res = tree.nodes[tree.root_idx].leaf_eval;

    // print_board(tree.sqr.root);
    // for (int i = 0; i < 100000; ++i) tree.playout();

    // tree.step();
    print_board(tree.sqr.root);
    // for (int i = 0; i < 100000; ++i) tree.playout();

    // print_root_moves(tree);

    std::cout << std::format("log: F={}, E={}, T={}\n", tree.node_log[0], tree.node_log[1], tree.node_log[2]);
    std::cout << std::format("tree.node_count() = {}\n", tree.nodes.size());

    // 
    // auto timer_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = timer_end - timer_start;
    std::cout << std::format("Finished ({:.1f}) in {:.3f}s with {} halfsteps.\n", res, elapsed.count(), ply);
    // std::cout << std::format("End.\n");
    return 0;
}
