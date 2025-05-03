#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "Board.hpp"
#include "Move.hpp"
#include "types.hpp"
#include "notation.hpp"

// Self‑play binary format structures ----------------------------------------
struct GameHeader {
    I8  result;      // +1 = white win, 0 = draw, -1 = black win
    U16 halfmoves;   // number of plies in the game
    U8  version;     // binary format version (currently 1)
};

struct PolicyItemBin {
    U16 move;        // encoded move
    U32 visits;      // number of MCTS visits for the move
};


// Loads one .bin file into the provided containers.
// Returns true on success, false if the file cannot be opened.
bool load_bin_game(
    const std::string&              path,
    std::vector<Board>&             boards,
    std::vector<I8>&                results,
    std::vector<float>&             values,
    std::vector<std::vector<Move>>& moves,
    std::vector<std::vector<U32>>&  visits)
{
    std::ifstream fin(path, std::ios::binary);
    if (!fin) return false;

    GameHeader hdr{};
    fin.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    if (hdr.version != 1)
        throw std::runtime_error("Unsupported .bin version");

    boards.clear(); results.clear(); values.clear();
    moves.clear();  visits.clear();

    boards.reserve(hdr.halfmoves);
    results.reserve(hdr.halfmoves);
    values.reserve(hdr.halfmoves);
    moves.reserve(hdr.halfmoves);
    visits.reserve(hdr.halfmoves);

    Board pos; // initial position

    for (U16 ply = 0; ply < hdr.halfmoves; ++ply) {

        // Read move code, value, and policy length
        U16 move_code;
        float value;
        U8 policy_len;
        fin.read(reinterpret_cast<char*>(&move_code), sizeof(move_code));
        fin.read(reinterpret_cast<char*>(&value),     sizeof(value));
        fin.read(reinterpret_cast<char*>(&policy_len), 1);

        // Read policy array
        std::vector<PolicyItemBin> policy(policy_len);
        if (policy_len)
            fin.read(reinterpret_cast<char*>(policy.data()),
                     policy_len * sizeof(PolicyItemBin));

        // Store snapshot before the move
        boards.push_back(pos);

        // Sign‑flipped result for the side to move
        I8 side_sign = (pos.side_to_move) ? 1 : -1;
        results.push_back(static_cast<I8>(hdr.result * side_sign));

        values.push_back(value);

        // Unpack policy
        std::vector<Move> move_vec;
        std::vector<U32>  visit_vec;
        move_vec.reserve(policy_len);
        visit_vec.reserve(policy_len);
        for (const auto& p : policy) {
            move_vec.emplace_back(Move{p.move});
            visit_vec.emplace_back(p.visits);
        }
        moves.push_back(move_vec);
        visits.push_back(visit_vec);

        // Advance position with the constructor Board(prev, move)
        Move played(move_code);
        Board next(pos, played);
        pos = next;
    }
    return true;
}



int main(int argc, char* argv[])
{
    const std::string game_path = "data/bin/"+ std::string(argv[1]) + ".bin";

    std::vector<Board> boards;
    std::vector<I8> results;
    std::vector<float> values;
    std::vector<std::vector<Move>> moves;
    std::vector<std::vector<U32>> visits;

    if (!load_bin_game(game_path, boards, results, values, moves, visits)) {
        std::cout << "Can't open " << game_path << '\n';
    }
    std::cout << "Loaded " << boards.size() << " positions\n";
    
    int j = boards.size()-1;
    // for (int j = 0; j < boards.size(); ++j){
        std::cout << "Value =  " << float(values[j]) << '\n';
        std::cout << "Result = " << int(results[j]) << '\n';
        print_board(boards[j]);
    // }

    return 0;
}