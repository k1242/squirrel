#pragma once
#include <string>
#include "Board.hpp"

extern const int PROMO_WHITE[4];
extern const int PROMO_BLACK[4];

// Convert square index (0..63) to 64-bit mask
inline constexpr uint64_t square(int index) {
    return 1ULL << index;
}

int square_index(const std::string& square);
std::string square_name(int index);

int piece_index(char piece);
char piece_char(int index);

int promotion_flag(char promotion_char);
char promotion_suffix(int flag);

void print_board(const Board& board);