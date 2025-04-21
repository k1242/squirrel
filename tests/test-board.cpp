#include <iostream>
#include "Board.hpp"
#include "Move.hpp"
#include "notation.hpp"
#include "movegen.hpp"
#include <cstdint>

int main() {
    std::cout << std::endl;

    // Board board;

    // fen en_passant black
    // Board board("rnbqkbnr/1ppppppp/p7/4P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    // board = Board(board, Move::from_uci("h7h5")); // here 
    // board = Board(board, Move::from_uci("d7d5"));  // not here
    
    // fen en_passant white
    // Board board("r1bqkb1r/N1ppp2p/2n3pn/6N1/5pP1/8/PPPPPP1P/R1BQKBR1 b Qkq g3 0 9");

    // en_passant 
    Board board("3q1br1/r3k2p/n2p1n2/pPpPpp2/1P3Np1/2BK4/PR1N1P1P/2Q1RB2 w - - 10 34");
    board = Board(board, Move::from_uci("h2h4"));
    board = Board(board, Move::from_uci("g4h3"));

    // castling
    // Board board("r3k2r/ppp1qp1p/1n1p1np1/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R3K2R w KQkq - 0 10");
    // board = Board(board, Move::from_uci("e1g1")); // w: O-O
    // board = Board(board, Move::from_uci("e1c1")); // w: O-O-O
    // board = Board(board, Move::from_uci("e8g8")); // b: O-O
    // board = Board(board, Move::from_uci("e8c8")); // b: O-O-O

    // kill the king rook
    // Board board("4kb1r/2rb1p2/1pn1p1Bp/p1p2q2/P5P1/BP2P2N/1Q1PKP1P/RN2R3 w k - 2 20");
    // board = Board(board, Move::from_uci("b2h8"));

    // Board board;
    // board = Board(board, Move::from_uci("e2e4"));
    // board = Board(board, Move::from_uci("e7e5"));
    // board = Board(board, Move::from_uci("d2d4"));
    // board = Board(board, Move::from_uci("f8c5"));
    // Board board("n1k5/1pppp1P1/8/8/8/5P2/pPPPP2P/1NBQKBNR b - - 10 1");

    auto occupancies = get_occupancies(board);
    
    U64 attacked = 0ULL;
    for (int sq = 0; sq < 64; ++sq)
    {
        if (is_square_attacked(board, occupancies, sq, board.side_to_move)) attacked |= square(sq);
    }

    std::vector<Move> moves;

    generate_pawn_moves(board, occupancies, moves);

    print_board(board);
    // print_bitboard(attacked);

    print_moves(moves);


    std::cout << std::endl;
    return 0;
}
