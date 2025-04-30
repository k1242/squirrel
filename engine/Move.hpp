#pragma once
#include <cstdint>
#include <string>
#include "types.hpp"

// Bits 12–15 of the Move data encoding are used to indicate move type:
//
// 0000(0) → normal move
// 0001(1) → kingside castling O-O
// 0010(2) → queenside castling O-O-O
// 0011(3) → en passant capture
//
// 0100(4) → promotion to queen
// 0101(5) → promotion to knight
// 0110(6) → promotion to rook
// 0111(7) → promotion to bishop

class Move {
public:
    Move() : data(0) {};
    Move(int from, int to, int flags = 0);
    static Move from_uci(const std::string& move_str);

    int from() const;
    int to() const;
    int flags() const;

    std::string uci() const;

    U16 data;
};
