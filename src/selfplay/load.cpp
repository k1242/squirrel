#include "load.hpp"
#include <fstream>
#include <stdexcept>

bool load_bin_game(const std::string&              path,
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
        throw std::runtime_error("unsupported .bin version");

    boards.clear(); results.clear(); values.clear();
    moves.clear();  visits.clear();

    boards.reserve(hdr.halfmoves);
    results.reserve(hdr.halfmoves);
    values.reserve(hdr.halfmoves);
    moves.reserve(hdr.halfmoves);
    visits.reserve(hdr.halfmoves);

    Board pos; // initial position
    for (U16 ply = 0; ply < hdr.halfmoves; ++ply) {
        U16 move_code;
        float value;
        U8 policy_len;
        fin.read(reinterpret_cast<char*>(&move_code), sizeof(move_code));
        fin.read(reinterpret_cast<char*>(&value),     sizeof(value));
        fin.read(reinterpret_cast<char*>(&policy_len), 1);

        std::vector<PolicyItemBin> policy(policy_len);
        if (policy_len)
            fin.read(reinterpret_cast<char*>(policy.data()),
                     policy_len * sizeof(PolicyItemBin));

        boards.push_back(pos);

        I8 side_sign = pos.side_to_move ? 1 : -1;
        results.push_back(static_cast<I8>(hdr.result * side_sign));

        values.push_back(value);

        std::vector<Move> mv;
        std::vector<U32>  vis;
        mv.reserve(policy_len);
        vis.reserve(policy_len);
        for (const auto& p : policy) {
            mv.emplace_back(Move{p.move});
            vis.emplace_back(p.visits);
        }
        moves.push_back(std::move(mv));
        visits.push_back(std::move(vis));

        Move played(move_code);
        pos = Board(pos, played);
    }
    return true;
}
