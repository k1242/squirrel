#pragma once
#include <string>
#include "Board.hpp"
#include "types.hpp"

extern const int PROMO_WHITE[4];
extern const int PROMO_BLACK[4];

inline constexpr U64 square(int index) {
    return 1ULL << index;
} 

int square_index(const std::string& square);
std::string square_name(int index);

int piece_index(char piece);
char piece_char(int index);

int promotion_flag(char promotion_char);
char promotion_suffix(int flag);

void print_board(const Board& board);
void print_bitboard(const U64& bb);
void print_moves(const std::vector<Move>& moves);

inline constexpr int mirror_sq   (int sq)  { return sq ^ 56; }
inline constexpr int mirror_piece(int p)   { return (p < 6) ? p + 6 : p - 6; }