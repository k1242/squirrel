#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include "Move.hpp"

// Board square index:
//   a  b  c  d  e  f  g  h
// 8 56 57 58 59 60 61 62 63
// 7 48 49 50 51 52 53 54 55
// 6 40 41 42 43 44 45 46 47
// 5 32 33 34 35 36 37 38 39
// 4 24 25 26 27 28 29 30 31
// 3 16 17 18 19 20 21 22 23
// 2  8  9 10 11 12 13 14 15
// 1  0  1  2  3  4  5  6  7

// Piece index:
// 0 : P, 1 : N, 2 : B, 3 : R, 4 : Q, 5 : K
// 6 : p, 7 : n, 8 : b, 9 : r, 10: q, 11: k

// Castling flag: 
// bit: 3210
//      qkQK

class Board {
public:
    Board();
    Board(const std::string& fen);
    Board(const Board& prev, const Move& move);

    void parse_fen(const std::string& fen);
    // std::vector<Move> legal_moves() const;

    // bool white_to_move() const { return side_to_move; }

    std::array<uint64_t, 12> bitboards;

    bool side_to_move;
    uint8_t castling_rights;
    int8_t en_passant_square;
    uint16_t halfmove_clock;
    uint16_t fullmove_number;

    static const uint64_t CASTLING_MASKS[4];
    static const uint64_t EN_PASSANT_MASKS[16];

    std::string fen() const;
};
