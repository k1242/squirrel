#include "Squirrel.hpp"
#include "notation.hpp"
#include <algorithm>
#include <bit>
// #include <format>
#include <cassert>

// constructor
Squirrel::Squirrel(const std::string& dir)
    : B1p(dir + "/B1p-32-256-w.npy", dir + "/B1p-32-a.npy"),
      B1o(dir + "/B1o-32-256-w.npy", dir + "/B1o-32-a.npy"),
      B2 (dir + "/B2-32-64-w.npy",   dir + "/B2-32-a.npy"),
      C1 (dir + "/C1-1-32-w.npy",    dir + "/C1-1-a.npy"),
      A1 (dir + "/A1-32-32-w.npy",   dir + "/A1-32-a.npy"),
      A2 (dir + "/A2-1-32-w.npy",    dir + "/A2-1-a.npy")
{
    // load base embedding
    B0w = lin::loadm(dir + "/B0-768-256-w.npy", hB0w);
    B0a = lin::loadv(dir + "/B0-256-a.npy",     hB0a);

    // load actor lookup matrix
    A0w = lin::loadm(dir + "/A0-768-32-w.npy", hA0w);
    
    moves.reserve(218);
    policy_logits_.reserve(218);
}

void Squirrel::set_root(const Board& root_pos)
{
    root = root_pos;

    for (int i = 0; i < DIM; ++i){
        accW_root[i] = 0.0f;
        accB_root[i] = 0.0f;
    }

    // constant shift
    lin::vavi(accW_root, B0a);
    lin::vavi(accB_root, B0a);

    // embed all pieces of the root position
    for (int p = 0; p < 12; ++p) {
        U64 bb = root.bitboards[p];
        while (bb) {
            int sq = std::countr_zero(bb);
            bb &= bb - 1;
            lin::vami(accW_root, B0w, p * 64 + sq);
            lin::vami(accB_root, B0w, mirror_piece(p) * 64 + mirror_sq(sq));
        }
    }

    reset();
}


// helpers to touch piece features
void Squirrel::add_piece_feature(int piece, int sq)
{
    int fW = piece * 64 + sq;
    lin::vami(accW_node, B0w, fW);

    int fB = mirror_piece(piece) * 64 + mirror_sq(sq);
    lin::vami(accB_node, B0w, fB);
}

void Squirrel::remove_piece_feature(int piece, int sq)
{
    int fW = piece * 64 + sq;
    lin::vsmi(accW_node, B0w, fW);

    int fB = mirror_piece(piece) * 64 + mirror_sq(sq);
    lin::vsmi(accB_node, B0w, fB);
}

void Squirrel::make(const Move& m)
{
    const int from  = m.from();
    const int to    = m.to();
    const U64 mask_from = square(from);
    const U64 mask_to   = square(to);
    const int flags     = m.flags();
    const bool side     = node.side_to_move;

    // locate moving piece
    int moving = -1;
    for (int p = side ? 0 : 6; p < (side ? 6 : 12); ++p)
        if (node.bitboards[p] & mask_from) {
            moving = p;
            node.bitboards[p] &= ~mask_from;
            remove_piece_feature(p, from);
            break;
        }

    assert(moving >= 0);

    // captures
    for (int p = side ? 6 : 0; p < (side ? 12 : 6); ++p)
        if (node.bitboards[p] & mask_to) {
            node.bitboards[p] &= ~mask_to;
            remove_piece_feature(p, to);
            node.halfmove_clock = 0;
            if (p == (side ? 9 : 3)) {
                if (to == (side ? 56 : 0))
                    node.castling_rights &= ~(side ? 0b1000 : 0b0010);
                else if (to == (side ? 63 : 7))
                    node.castling_rights &= ~(side ? 0b0100 : 0b0001);
            }
            break;
        }

    // pawn specifics
    bool pawn_move = (moving == 0 || moving == 6);
    if (pawn_move) node.halfmove_clock = 0;

    // en-passant capture
    if (pawn_move && to == node.en_passant_square) {
        int cap_sq = side ? to - 8 : to + 8;
        node.bitboards[side ? 6 : 0] &= ~square(cap_sq);
        remove_piece_feature(side ? 6 : 0, cap_sq);
    }

    // promotion
    if (pawn_move && flags >= 4 && flags <= 7)
        moving = side ? PROMO_WHITE[flags - 4] : PROMO_BLACK[flags - 4];

    node.en_passant_square = -1;
    if (pawn_move && std::abs(to - from) == 16)
        node.en_passant_square = side ? from + 8 : from - 8;

    // Update castling rights for qkQK
    uint8_t castling_upd = 0xFF;
    if (moving == (side ? 5 : 11)) {
        // King moved → remove both castling rights
        castling_upd ^= (side ? 0b0011 : 0b1100);
    } else if (moving == (side ? 3 : 9)) {
        // Rook moved from starting square
        if (from == (side ? 0 : 56))      // a1 / a8
            castling_upd ^= (side ? 0b0010 : 0b1000);
        else if (from == (side ? 7 : 63)) // h1 / h8
            castling_upd ^= (side ? 0b0001 : 0b0100);
    }
    node.castling_rights &= castling_upd;

    // Handle castling moves
    if (moving == (side ? 5 : 11) && std::abs(to - from) == 2) {
        const int rook_from = (to > from) ? (from + 3) : (from - 4);
        const int rook_to   = (to > from) ? (from + 1) : (from - 1);
        const int rook_piece = side ? 3 : 9;

        node.bitboards[rook_piece] &= ~square(rook_from);
        remove_piece_feature(rook_piece, rook_from);

        node.bitboards[rook_piece] |= square(rook_to);
        add_piece_feature(rook_piece, rook_to);
    }

    // place mover
    node.bitboards[moving] |= mask_to;
    add_piece_feature(moving, to);

    node.side_to_move   = !side;
    ++node.halfmove_clock;
    if (!side) ++node.fullmove_number;
}

void Squirrel::reset()
{
    node = root;
    std::copy(bufW_root, bufW_root + DIM, bufW_node);
    std::copy(bufB_root, bufB_root + DIM, bufB_node);
}

void Squirrel::jump(const Move& m)
{
    make(m);
    root = node;
    std::copy(bufW_node, bufW_node + DIM, bufW_root);
    std::copy(bufB_node, bufB_node + DIM, bufB_root);
}

// Body
void Squirrel::encode_body()
{
    bool side = node.side_to_move;

    for (int i = 0; i < DIM; ++i) {
        float own  = acc(side,  i);
        float oppo = acc(!side, i);
        xo[i] = own  > 0.0f ? own  : 0.0f;
        xp[i] = oppo > 0.0f ? oppo : 0.0f;
    }

    B1p(xp); B1p.relu();
    B1o(xo); B1o.relu();

    for (int i = 0; i < 32; ++i) {
        xj[i]      = B1p.inner[i];
        xj[i + 32] = B1o.inner[i];
    }

    B2(xj); B2.relu();
}

// Critic
void Squirrel::run_critic()
{
    C1(B2.inner);
    value_ = C1.inner[0];
}

// Actor
void Squirrel::run_actor()
{
    A1(B2.inner);

    bool side = node.side_to_move;

    policy_logits_.assign(moves.size(), 0.0f);
    
    for (int k = 0; k < moves.size(); ++k) {
        const Move& m = moves[k];
        int from = m.from();
        int to   = m.to();
        int moving = -1;
        U64 mask_from = square(from);

        for (int p = side ? 0 : 6; p < (side ? 6 : 12); ++p) {
            if (node.bitboards[p] & mask_from) { 
                moving = p; 
                break; 
            }
        }

        for (int i = 0; i < 32; ++i)
            xm[i] = 0.0f;

        assert(moving >= 0);
        lin::vsmi(xm, A0w, moving * 64 + from);
        lin::vami(xm, A0w, moving * 64 + to);

        lin::vavi(xm, A1.inner);
        for (int i = 0; i < 32; ++i)
            xm[i] = (xm[i] < 0.0f) ? 0.0f : xm[i];

        A2(xm);
        policy_logits_[k] = A2.inner[0];
    }
}

void Squirrel::eval()
{
    moves = node.legal_moves();
    is_terminal(node, moves, terminal, value_);

    if (!terminal) {
        encode_body();
        run_critic();
        // run_actor();
        // value_ = 0.0f;
        policy_logits_.assign(moves.size(), 0.0f);
    }

}
