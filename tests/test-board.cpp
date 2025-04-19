#include <iostream>
#include "Board.hpp"
#include "Move.hpp"
#include "notation.hpp"
#include <cstdint>

int main() {
    std::cout << std::endl;

    // fen en_passant black
    // Board board("rnbqkbnr/1ppppppp/p7/4P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    // board = Board(board, Move::from_uci("h7h5")); // here 
    // board = Board(board, Move::from_uci("d7d5"));  // not here
    
    // fen en_passant white
    // Board board("r1bqkb1r/N1ppp2p/2n3pn/6N1/5pP1/8/PPPPPP1P/R1BQKBR1 b Qkq g3 0 9");

    // en_passant 
    // Board board("3q1br1/r3k2p/n2p1n2/pPpPpp2/1P3Np1/2BK4/PR1N1P1P/2Q1RB2 w - - 10 34");
    // board = Board(board, Move::from_uci("h2h4"));
    // board = Board(board, Move::from_uci("g4h3"));

    // castling
    Board board("r3k2r/ppp1qp1p/1n1p1np1/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R3K2R w KQkq - 0 10");
    // board = Board(board, Move::from_uci("e1g1")); // w: O-O
    board = Board(board, Move::from_uci("e1c1")); // w: O-O-O
    board = Board(board, Move::from_uci("e8g8")); // b: O-O
    // board = Board(board, Move::from_uci("e8c8")); // b: O-O


    std::cout << "Side to move: " << (board.side_to_move ? "White" : "Black") << '\n';

    std::cout << "Castling rights: ";
    uint8_t cr = board.castling_rights;
    std::cout << ((cr & 1) ? "K" : "") << ((cr & 2) ? "Q" : "")
              << ((cr & 4) ? "k" : "") << ((cr & 8) ? "q" : "") << '\n';

    std::cout << "En passant square: ";
    if (board.en_passant_square == -1) std::cout << "-\n";
    else std::cout << square_name(board.en_passant_square) << '\n';

    std::cout << "Halfmove clock: " << int(board.halfmove_clock) << '\n';
    std::cout << "Fullmove number: " << board.fullmove_number << '\n';

    print_board(board);

    std::cout << board.fen() << '\n'; 

    std::cout << std::endl;
    return 0;
}
