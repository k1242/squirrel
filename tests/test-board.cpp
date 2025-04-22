#include <iostream>
#include <cstdint>
#include <chrono>
#include "Board.hpp"
#include "Move.hpp"
#include "notation.hpp"
#include "movegen.hpp"


U64 perft(Board& board, int depth) {
    if (depth == 0) return 1;

    U64 nodes = 0ULL;
    std::vector<Move> moves = board.legal_moves();

    for (const Move& mv : moves) {
        Board next(board, mv);
        nodes += perft(next, depth - 1);
    }
    return nodes;
}


int main() {
    std::cout << std::endl;

    Board board;
    // Board board("5rk1/5pp1/7B/4N3/2B3Q1/8/8/4K2R b K - 0 1");
    // Board board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1 ");
    // Board board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    // Board board("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
    // Board board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    print_board(board);
    
    // std::vector<Move> moves = board.legal_moves();
    // std::cout << "# moves: " << moves.size() << std::endl;
    // print_moves(moves);

    int depth = 6;
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t nodes = perft(board, depth);
    auto stop  = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(stop - start).count();

    std::cout << "\nDepth " << depth << " nodes: " << nodes << '\n';
    std::cout << "Time:          " << ms/1000.0   << " s\n";
    std::cout << "NPS:           " << (nodes / (ms / 1000.0)) << '\n';

    std::cout << std::endl;
    return 0;
}
