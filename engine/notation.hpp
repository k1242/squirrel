#pragma once
#include <string>
#include "Board.hpp"

int square_index(const std::string& square);
std::string square_name(int index);

int piece_index(char piece);
char piece_char(int index);

int promotion_flag(char promotion_char);
char promotion_suffix(int flag);

void print_board(const Board& board);