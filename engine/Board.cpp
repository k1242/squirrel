#include "notation.hpp"
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <iostream>

const uint64_t Board::CASTLING_MASKS[4] = {
    square(61) | square(63), // 00. Black kingside
    square(5)  | square(7),  // 01. White kingside
    square(56) | square(59), // 10. Black queenside
    square(0)  | square(3)   // 11. White queenside
};

const uint64_t Board::EN_PASSANT_MASKS[16] = {
    // first 8 for white: a3, b3, ...
    // next  8 for black: a6, b6, ...
                 square(25),
    square(24) | square(26),
    square(25) | square(27),
    square(26) | square(28),
    square(27) | square(29),
    square(28) | square(30),
    square(29) | square(31),
    square(30),
                 square(33),
    square(32) | square(34),
    square(33) | square(35),
    square(34) | square(36),
    square(35) | square(37),
    square(36) | square(38),
    square(37) | square(39),
    square(38),
};

Board::Board() {
    parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board::Board(const std::string& fen) {
    parse_fen(fen);
}

Board::Board(const Board& prev, const Move& move)
    : bitboards(prev.bitboards),
      side_to_move(!prev.side_to_move),  // toggle side to move
      castling_rights(prev.castling_rights),
      en_passant_square(-1),             // default, to be updated later if needed
      halfmove_clock(prev.halfmove_clock + 1), // increment initially
      fullmove_number(prev.fullmove_number + (prev.side_to_move ? 0 : 1)) {

    const int from = move.from();
    const int to = move.to();
    const int flags = move.flags();

    // Determine moving piece and its color
    const bool is_white = prev.side_to_move;
    int moving_piece = -1;
    for (int piece = (is_white ? 0 : 6); piece < (is_white ? 6 : 12); ++piece) {
        if (bitboards[piece] & (1ULL << from)) {
            moving_piece = piece;
            bitboards[piece] &= ~(1ULL << from);   // remove piece from origin
            break;
        }
    }

    // Handle captures (if any)
    for (int piece = (is_white ? 6 : 0); piece < (is_white ? 12 : 6); ++piece) {
        if (bitboards[piece] & (1ULL << to)) {
            bitboards[piece] &= ~(1ULL << to);     // remove captured piece
            halfmove_clock = 0;                    // reset halfmove_clock due to capture
            break;
        }
    }

    // Handle pawn moves and promotions
    if (moving_piece == 0 || moving_piece == 6) {
        halfmove_clock = 0;  // reset halfmove_clock for pawn moves
        if (flags >= 4 && flags <= 7) {
            // Promotion
            moving_piece = is_white ? PROMO_WHITE[flags - 4] : PROMO_BLACK[flags - 4];
        } else if (to == prev.en_passant_square) {
            // Handle en passant capture
            std::cout << "Handle en passant capture" << std::endl;
            int ep_capture_sq = is_white ? (to - 8) : (to + 8);
            bitboards[is_white ? 6 : 0] &= ~(1ULL << ep_capture_sq);  // remove pawn captured en passant
        } else if (abs(to - from) == 16) {
            // Set new en passant square after double pawn push
            en_passant_square = is_white ? from + 8 : from - 8;
        }
    }

    // Update castling rights for qkQK
    uint8_t castling_upd = 0xFF;
    if (moving_piece == (is_white ? 5 : 11)) {
        // King moved → remove both castling rights
        castling_upd ^= (is_white ? 0b0011 : 0b1100);
    } else if (moving_piece == (is_white ? 3 : 9)) {
        // Rook moved from starting square
        if (from == (is_white ? 0 : 56))      // a1 / a8
            castling_upd ^= (is_white ? 0b0010 : 0b1000);
        else if (from == (is_white ? 7 : 63)) // h1 / h8
            castling_upd ^= (is_white ? 0b0001 : 0b0100);
    }
    castling_rights &= castling_upd;
    
    // Handle castling moves
    if (moving_piece == (is_white ? 5 : 11) && std::abs(to - from) == 2) {
        bitboards[is_white ? 3 : 9] ^= CASTLING_MASKS[(to < from) * 2 + is_white];
    }

    // Normal move
    bitboards[moving_piece] |= square(to);

}


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


std::string Board::fen() const
{
    // Piece placement
    auto piece_on = [&](int sq) -> char {
        for (int idx = 0; idx < 12; ++idx)
            if (bitboards[idx] & (1ULL << sq))
                return piece_char(idx);
        return ' ';
    };

    std::ostringstream fen;

    for (int rank = 7; rank >= 0; --rank) {
        int empty = 0;
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            char p = piece_on(sq);
            if (p == ' ') {
                ++empty;
            } else {
                if (empty) { fen << empty; empty = 0; }
                fen << p;
            }
        }
        if (empty) fen << empty;
        if (rank)  fen << '/';
    }

    // Side to move
    fen << ' ' << (side_to_move ? 'w' : 'b') << ' ';

    // Castling rights
    std::string cast;
    if (castling_rights & 0b0001) cast += 'K';
    if (castling_rights & 0b0010) cast += 'Q';
    if (castling_rights & 0b0100) cast += 'k';
    if (castling_rights & 0b1000) cast += 'q';
    fen << (cast.empty() ? "-" : cast) << ' ';


    // En‑passant square
    bool valid_ep = bitboards[(1-side_to_move)*6] & EN_PASSANT_MASKS[en_passant_square - side_to_move * 16 - 16];
    valid_ep &= (en_passant_square != -1);
    fen << (valid_ep ? square_name(en_passant_square) : "-") << ' ';
    // fen << (en_passant_square == -1 ? "-" : square_name(en_passant_square)) << ' ';

    //Halfmove clock and fullmove number
    fen << int(halfmove_clock) << ' ' << fullmove_number;

    return fen.str();
}