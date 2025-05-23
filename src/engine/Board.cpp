#include <sstream>
#include <cctype>
#include <stdexcept>
#include <bit>
#include "types.hpp"
#include "movegen.hpp"
#include "notation.hpp"

const U64 Board::CASTLING_MASKS[4] = {11529215046068469760ULL, 160ULL, 648518346341351424ULL, 9ULL};
const U64 Board::EN_PASSANT_MASKS[16] = {33554432ULL, 83886080ULL, 167772160ULL, 335544320ULL, 671088640ULL, 1342177280ULL, 2684354560ULL, 1073741824ULL, 8589934592ULL, 21474836480ULL, 42949672960ULL, 85899345920ULL, 171798691840ULL, 343597383680ULL, 687194767360ULL, 274877906944ULL};

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

    // PROFILE;

    const int from = move.from();
    const int to = move.to();
    const int flags = move.flags();

    const U64 mask_from = square(from);
    const U64 mask_to = square(to);

    // Determine moving piece and its color
    const bool is_white = prev.side_to_move;
    int moving_piece = -1;
    for (int piece = (is_white ? 0 : 6); piece < (is_white ? 6 : 12); ++piece) {
        if (bitboards[piece] & mask_from) {
            moving_piece = piece;
            bitboards[piece] &= ~mask_from;   // remove piece from origin
            break;
        }
    }

    // Handle captures (if any)
    for (int piece = (is_white ? 6 : 0); piece < (is_white ? 12 : 6); ++piece) {
        if (bitboards[piece] & mask_to) {
            bitboards[piece] &= ~mask_to;       // remove captured piece
            halfmove_clock = 0;                    // reset halfmove_clock due to capture
            if (piece == (is_white ? 9 : 3)) {
                if (to == (is_white ? 56 : 0)) {
                    castling_rights &= ~(is_white ? 0b1000 : 0b0010);
                } else if (to == (is_white ? 63 : 7)) {
                    castling_rights &= ~(is_white ? 0b0100 : 0b0001);
                }
            }
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
            bitboards[is_white ? 6 : 0] &= ~square(is_white ? (to - 8) : (to + 8));  // remove pawn captured en passant
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
    bitboards[moving_piece] |= mask_to;

    // ++halfmove_clock;
    // if (!is_white) ++fullmove_number;

}


void Board::parse_fen(const std::string& fen) {
    std::istringstream ss(fen);
    std::string piece_field, turn, castling, ep;
    ss >> piece_field >> turn >> castling >> ep >> halfmove_clock >> fullmove_number;

    for (auto& b : bitboards) b = 0;
    int sq = 56;
    for (char c : piece_field) {
        if (c == '/') {
            sq -= 16;
        } else if (isdigit(c)) {
            sq += c - '0';
        } else {
            bitboards[piece_index(c)] |= square(sq);
            sq++;
        }
    }

    side_to_move = (turn == "w");
    castling_rights = (castling.find('K') != std::string::npos) << 0 |
                      (castling.find('Q') != std::string::npos) << 1 |
                      (castling.find('k') != std::string::npos) << 2 |
                      (castling.find('q') != std::string::npos) << 3;

    en_passant_square = (ep == "-") ? -1 : square_index(ep);
}

std::vector<Move> filter_legal_moves(const Board& board, const std::vector<Move>& pseudo) {
    // PROFILE;

    std::vector<Move> legal;
    legal.reserve(pseudo.size());

    for (const Move& move : pseudo) {
        Board next(board, move);
        if (!is_king_attacked(next, get_occupancies(next), 1 - next.side_to_move))
            legal.push_back(move);
    }

    return legal;
}


std::vector<Move> Board::legal_moves() const {
    // PROFILE;
    std::vector<Move> pseudo; pseudo.reserve(64);

    auto occupancies = get_occupancies(*this);
    generate_pawn_moves  (*this, occupancies, pseudo);
    generate_knight_moves(*this, occupancies, pseudo);
    generate_bishop_moves(*this, occupancies, pseudo);
    generate_rook_moves  (*this, occupancies, pseudo);
    generate_queen_moves (*this, occupancies, pseudo);
    generate_king_moves  (*this, occupancies, pseudo);

    return filter_legal_moves(*this, pseudo);

    // // keep only legal moves
    // std::vector<Move> legal; legal.reserve(64);
    // legal.reserve(pseudo.size());

    // for (const Move& move : pseudo) {
    //     Board next(*this, move);
    //     if (!is_king_attacked(next, get_occupancies(next), 1-next.side_to_move))
    //         legal.push_back(move);
    // }
    // return legal;
}


std::string Board::fen() const
{
    // Piece placement
    auto piece_on = [&](int sq) -> char {
        for (int idx = 0; idx < 12; ++idx)
            if (bitboards[idx] & square(sq))
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


bool Board::operator==(const Board& other) const {
    for (int i = 0; i < 12; ++i) {
        if (bitboards[i] != other.bitboards[i])
            return false;
    }
    return side_to_move == other.side_to_move
        && castling_rights == other.castling_rights
        && en_passant_square == other.en_passant_square
        && halfmove_clock == other.halfmove_clock
        && fullmove_number == other.fullmove_number;
}


void is_terminal(const Board& board,
                 const std::vector<Move>& moves,
                 bool& terminal,
                 float& value)
{
    terminal = false;

    // checkmate or stalemate
    if (moves.empty()) {
        bool in_check = is_king_attacked(board, get_occupancies(board), board.side_to_move);
        terminal = true;
        // value = in_check ? (1.0f - 2.0f * static_cast<float>(board.side_to_move)) : 0.0f;
        value = in_check ? 1.0f : 0.0f;
        return;
    }

    // fifty-move rule
    if (board.halfmove_clock >= 100) {
        terminal = true;
        value = 0.0f;
        return;
    }

    // insufficient material
    U64 pawns  = board.bitboards[0] | board.bitboards[6];
    U64 rooks  = board.bitboards[3] | board.bitboards[9];
    U64 queens = board.bitboards[4] | board.bitboards[10];

    if (pawns | rooks | queens) return; // material still sufficient

    U64 bishops = board.bitboards[2] | board.bitboards[8];
    U64 knights = board.bitboards[1] | board.bitboards[7];
    int minors  = std::popcount(bishops | knights);

    if (minors <= 1) {                         // K vs K / K+minor vs K
        terminal = true;
        value = 0.0f;
        return;
    }

    const U64 dark = 0xAA55AA55AA55AA55ULL;        // dark squares

    auto side_draw = [&](U64 bishops, U64 knights) {
        int b = std::popcount(bishops);
        int n = std::popcount(knights);
        if (b + n > 2) return false;               // too much material
        if (b && n)    return false;               // bishop + knight can mate
        if (b == 2) {                              // two bishops
            bool opposite = (bishops & dark) && (bishops & ~dark);
            if (opposite) return false;            // opposite colours can mate
        }
        return true;                               // otherwise draw
    };

    bool white_ok = side_draw(board.bitboards[2], board.bitboards[1]);
    bool black_ok = side_draw(board.bitboards[8], board.bitboards[7]);

    if (white_ok && black_ok) {
        terminal = true;
        value = 0.0f;
        return;
    }
}
