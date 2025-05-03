#include "load.hpp"
#include "notation.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    std::string id   = argv[1];
    std::string path = "data/games/" + id + ".bin";

    std::vector<Board> boards;
    std::vector<I8> results;
    std::vector<float> values;
    std::vector<std::vector<Move>> moves;
    std::vector<std::vector<U32>>  visits;

    if (!load_bin_game(path, boards, results, values, moves, visits)) {
        std::cerr << "can't open " << path << '\n';
        return 1;
    }

    std::cout << "loaded " << boards.size() << " plies from " << path << '\n';

    // simple smoke: покажем последний снимок
    if (!boards.empty()) {
        std::cout << "last value  = " << values.back()  << '\n';
        std::cout << "last result = " << int(results.back()) << '\n';
        print_board(boards.back());
    }
    return 0;
}
