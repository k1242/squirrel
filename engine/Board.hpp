#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include "Move.hpp"

class Board {
public:
    Board();
    explicit Board(const std::string& fen);
    Board(const Board& prev, const Move& move);

    void parse_fen(const std::string& fen);
    std::vector<Move> legal_moves() const;

    bool white_to_move() const { return side_to_move; }

    std::array<uint64_t, 12> bitboards;
    bool side_to_move;
    uint8_t castling_rights;
    int8_t en_passant_square;
    uint8_t halfmove_clock;
    uint16_t fullmove_number;
};
