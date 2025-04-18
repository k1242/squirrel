#include "notation.hpp"
#include <sstream>
#include <cctype>
#include <stdexcept>

Board::Board() {
    parse_fen("rn1qkbnr/pppb1ppp/8/3pp3/3PP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1");
}

Board::Board(const std::string& fen) {
    parse_fen(fen);
}

// Board::Board(const Board& prev, const Move& move)
//     : bitboards(prev.bitboards),
//       side_to_move(prev.side_to_move),
//       castling_rights(prev.castling_rights),
//       en_passant_square(-1),
//       halfmove_clock(prev.halfmove_clock),
//       fullmove_number(prev.fullmove_number + (prev.side_to_move ? 0 : 1)) {
//     // TODO: implement move logic
// }

void Board::parse_fen(const std::string& fen) {
    std::istringstream ss(fen);
    std::string piece_field, turn, castling, ep;
    ss >> piece_field >> turn >> castling >> ep >> halfmove_clock >> fullmove_number;

    for (auto& b : bitboards) b = 0;
    int sq = 56;
    for (char c : piece_field) {
        if (c == '/') sq -= 16;
        else if (isdigit(c)) sq += c - '0';
        else bitboards[piece_index(c)] |= 1ULL << sq++;
    }

    side_to_move = (turn == "w");
    castling_rights = (castling.find('K') != std::string::npos) << 0 |
                      (castling.find('Q') != std::string::npos) << 1 |
                      (castling.find('k') != std::string::npos) << 2 |
                      (castling.find('q') != std::string::npos) << 3;

    en_passant_square = (ep == "-") ? -1 : square_index(ep);
}

// std::vector<Move> Board::legal_moves() const {
//     return {};  // TODO: implement
// }
