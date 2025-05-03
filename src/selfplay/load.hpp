#pragma once
#include <string>
#include <vector>
#include "format.hpp"
#include "Board.hpp"
#include "Move.hpp"

// load .bin, return false if file can't be opened
bool load_bin_game(
    const std::string&              path,
    std::vector<Board>&             boards,
    std::vector<I8>&                results,
    std::vector<float>&             values,
    std::vector<std::vector<Move>>& moves,
    std::vector<std::vector<U32>>&  visits);
