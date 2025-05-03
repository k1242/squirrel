#pragma once

#include "types.hpp"
#include "Board.hpp"

// Attack generation functions
inline U64 get_bishop_attacks(int sq, U64 occupancy);
inline U64 get_rook_attacks(int sq, U64 occupancy);
inline U64 get_queen_attacks(int sq, U64 occupancy);

// Attack check function
int is_square_attacked(const Board& board, const std::array<U64, 3>& occupancies, int sq, int side);
int is_king_attacked(const Board& board, const std::array<U64, 3>& occupancies, int side);

std::array<U64, 3> get_occupancies(const Board& board);

void generate_pawn_moves(const Board& board, const std::array<U64, 3>& occupancies, std::vector<Move>& move_list);
void generate_knight_moves(const Board& board, const std::array<U64, 3>& occupancies, std::vector<Move>& move_list);
void generate_bishop_moves(const Board& board, const std::array<U64, 3>& occupancies, std::vector<Move>& move_list);
void generate_rook_moves(const Board& board, const std::array<U64, 3>& occupancies, std::vector<Move>& move_list);
void generate_queen_moves(const Board& board, const std::array<U64, 3>& occupancies, std::vector<Move>& move_list);
void generate_king_moves(const Board& board, const std::array<U64, 3>& occupancies, std::vector<Move>& move_list);

U64 perft(Board& board, int depth);