#include "Squirrel.hpp"
#include "notation.hpp"
#include <algorithm>
#include <bit>
#include <format>

// constructor
Squirrel::Squirrel(const std::string& dir, const Board& root_pos)
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

    root = root_pos;

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
    moves.reserve(218);
    policy_logits_.reserve(218);
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
    node.side_to_move   = !side;

    ++node.halfmove_clock;
    if (!side) ++node.fullmove_number;

    // locate moving piece
    int moving = -1;
    for (int p = side ? 0 : 6; p < (side ? 6 : 12); ++p)
        if (node.bitboards[p] & mask_from) {
            moving = p;
            node.bitboards[p] &= ~mask_from;
            remove_piece_feature(p, from);
            break;
        }

    // captures
    for (int p = side ? 6 : 0; p < (side ? 12 : 6); ++p)
        if (node.bitboards[p] & mask_to) {
            node.bitboards[p] &= ~mask_to;
            remove_piece_feature(p, to);
            node.halfmove_clock = 0;
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

    // place mover
    node.bitboards[moving] |= mask_to;
    add_piece_feature(moving, to);
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

        // std::cout << std::format("{} : {}({}) → {}({})\n", moving, from, moving * 64 + from, to, moving * 64 + to);

        lin::vsmi(xm, A0w, moving * 64 + from);
        lin::vami(xm, A0w, moving * 64 + to);

        lin::vavi(xm, A1.inner);
        for (int i = 0; i < 32; ++i)
            xm[i] = (xm[i] < 0.0f) ? 0.0f : xm[i];

        A2(xm);
        policy_logits_[k] = A2.inner[0];
    }
}

void Squirrel::evaluate()
{
    moves = node.legal_moves();
    
    encode_body();
    run_critic();
    run_actor();
}
