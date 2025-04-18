#include <iostream>
#include "Board.hpp"
#include "notation.hpp"

int main() {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "Side to move: " << (board.white_to_move() ? "White" : "Black") << '\n';

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

    return 0;
}
