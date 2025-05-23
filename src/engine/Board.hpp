#pragma once
#include <array>
#include <string>
#include <vector>
#include "Move.hpp"
#include "types.hpp"

// Board square index:
//     a  b  c  d  e  f  g  h
//      
// 8   56 57 58 59 60 61 62 63
// 7   48 49 50 51 52 53 54 55
// 6   40 41 42 43 44 45 46 47
// 5   32 33 34 35 36 37 38 39
// 4   24 25 26 27 28 29 30 31
// 3   16 17 18 19 20 21 22 23
// 2    8  9 10 11 12 13 14 15
// 1    0  1  2  3  4  5  6  7

// Piece index:
// 0 : P, 1 : N, 2 : B, 3 : R, 4 : Q, 5 : K
// 6 : p, 7 : n, 8 : b, 9 : r, 10: q, 11: k

// Castling flag: 
// bit: 3210
//      qkQK

// Side to move:
// false : black
// true  : white

// struct Undo {
//     Move move;
//     int captured_piece;
//     uint8_t castling_rights;
//     int en_passant_square;
//     uint8_t halfmove_clock;
// };

class Board {
public:
    Board();
    Board(const std::string& fen);
    Board(const Board& prev, const Move& move);

    void parse_fen(const std::string& fen);

    std::vector<Move> legal_moves() const;

    std::array<U64, 12> bitboards;

    bool side_to_move;
    U8 castling_rights;
    I8 en_passant_square;
    U16 halfmove_clock;
    U16 fullmove_number;

    static const U64 CASTLING_MASKS[4];
    static const U64 EN_PASSANT_MASKS[16];

    std::string fen() const;

    bool operator==(const Board& other) const;
    bool operator!=(const Board& other) const { return !(*this == other); }
};


void is_terminal(const Board& board,
                 const std::vector<Move>& moves,
                 bool& terminal,
                 float& value);