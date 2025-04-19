#include "notation.hpp"
#include <stdexcept>
#include <iostream>
#include "Board.hpp"

const int PROMO_WHITE[4] = {4, 1, 3, 2};   // Q N R B
const int PROMO_BLACK[4] = {10, 7, 9, 8};  // q n r b

int square_index(const std::string& square) {
    if (square.size() != 2) throw std::invalid_argument("Invalid square");
    char file = square[0], rank = square[1];
    if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
        throw std::invalid_argument("Invalid square");
    return (rank - '1') * 8 + (file - 'a');
}

std::string square_name(int index) {
    if (index < 0 || index > 63) throw std::invalid_argument("Invalid index");
    char file = 'a' + (index % 8);
    char rank = '1' + (index / 8);
    return std::string{file, rank};
}

int piece_index(char piece) {
    switch (piece) {
        case 'P': return 0; case 'N': return 1; case 'B': return 2;
        case 'R': return 3; case 'Q': return 4; case 'K': return 5;
        case 'p': return 6; case 'n': return 7; case 'b': return 8;
        case 'r': return 9; case 'q': return 10; case 'k': return 11;
        default: throw std::invalid_argument("Invalid piece char");
    }
}

char piece_char(int index) {
    const char pieces[] = "PNBRQKpnbrqk";
    if (index < 0 || index > 11) throw std::invalid_argument("Invalid piece index");
    return pieces[index];
}

int promotion_flag(char promotion_char) {
    switch (promotion_char) {
        case 'q': return 4; case 'n': return 5;
        case 'r': return 6; case 'b': return 7;
        default: throw std::invalid_argument("Invalid promotion char");
    }
}

char promotion_suffix(int flag) {
    switch (flag) {
        case 4: return 'q'; case 5: return 'n';
        case 6: return 'r'; case 7: return 'b';
        default: return '\0';
    }
}

void print_board(const Board& board) {
    const char* symbols = "PNBRQKpnbrqk";
    char squares[64];
    for (char& c : squares) c = '.';

    for (int i = 0; i < 12; ++i) {
        uint64_t bb = board.bitboards[i];
        for (int sq = 0; sq < 64; ++sq) {
            if (bb & (1ULL << sq)) squares[sq] = symbols[i];
        }
    }

    std::cout << "Board:\n";
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file)
            std::cout << squares[rank * 8 + file] << ' ';
        std::cout << '\n';
    }
}