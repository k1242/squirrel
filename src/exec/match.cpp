// src/exec/match_models.cpp
#include "Board.hpp"
#include "Squirrel.hpp"
#include "Tree.hpp"
#include "movegen.hpp"
#include "notation.hpp"
#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <numeric>
#include <iostream>
#include <iomanip>


using Clock = std::chrono::high_resolution_clock;

// choose random legal move
static Move random_move(const std::vector<Move>& moves, std::mt19937& rng)
{
    std::uniform_int_distribution<int> dist(0, (int)moves.size() - 1);
    return moves[dist(rng)];
}

static Board get_random_pos(int R, std::mt19937& rng)
{
    Board pos;

    while (true) {
        Board cur = pos;
        for (int ply = 0; ply < R; ++ply) {
            std::vector<Move> moves = cur.legal_moves();   // all legal moves
            if (moves.empty()) {                           // stalemate / mate
                cur = Board();                             // restart from start
                ply = -1;                                  // will become 0
                continue;
            }
            cur = Board(cur, random_move(moves, rng));
        }
        if (!cur.legal_moves().empty())
            return cur;
    }
}



// play single game, return result from White viewpoint (+1 / 0.5 / 0)
static float play_game(const Board& start_pos,
                       Squirrel& net_w, Squirrel& net_b,
                       int playouts, std::mt19937& rng)
{
    Board pos = start_pos;
    Squirrel* nets[2] = { &net_w, &net_b };
    int cntr = 0;

    while (true) {
        cntr++;
        // terminal test
        bool term; float value;
        std::vector<Move> tmp = pos.legal_moves();
        is_terminal(pos, tmp, term, value);
        if (term) {
            // print_board(pos);
            // std::cout << "played " << cntr << " moves: " << pos.side_to_move << " " << value;
            if (value > 0.5f) {
                value = (pos.side_to_move) ? 0.0f : 1.0f; 
            } else {
                value = 0.5f; 
            }
            // std::cout << " " << value << "\n";
            return value;
        }

        // MCTS tree for current side
        Tree tree(*nets[pos.side_to_move]);
        tree.new_game(pos);
        for (int i = 0; i < playouts; ++i) tree.playout();

        int best = tree.best_child(false);
        assert(best != -1);

        tree.step(best);                      // sync board inside tree
        pos = tree.sqr.root;                  // take new root board
    }
}

int main(int argc, char* argv[])
{
    std::string dirA = argv[1];
    std::string dirB = argv[2];
    int R           = (argc >= 4) ? std::stoi(argv[3]) : 10;
    int playouts    = (argc >= 5) ? std::stoi(argv[4]) : 1000;

    // initialise nets
    Squirrel netA(dirA);
    Squirrel netB(dirB);

    // random start position
    std::mt19937 rng{ std::random_device{}() };
    Board root_pos1 = get_random_pos(R, rng);
    Board root_pos2 = root_pos1;

    // print_board(root_pos1);

    // Game 1: A white, B black
    float res1 = play_game(root_pos1, netA, netB, playouts, rng);

    // Game 2: roles reversed
    float res2 = 1.0f - play_game(root_pos2, netB, netA, playouts, rng);

    // statistics
    int wA = (res1 > 0.75f) + (res2 > 0.75f);
    int wB = (res1 < 0.25f) + (res2 < 0.25f);
    int dr = 2 - wA - wB;

    std::cout << "\nresults:  A wins " << wA
              << ", B wins " << wB
              << ", draws "   << dr << '\n';

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "score:    A = " << (res1 + res2) << ", B = " << (2.0f - (res1 + res2)) << '\n';

    return 0;
}
