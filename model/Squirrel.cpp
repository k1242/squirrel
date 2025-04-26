#include <algorithm>   // copy
#include "notation.hpp"
#include <bit> // countr_zero

void Squirrel::add_piece_feature(int piece, int sq)
{
    int fW = piece * 64 + sq;
    lin::vami(accW_node, W, fW);

    int fB = mirror_piece(piece) * 64 + mirror_sq(sq);
    lin::vami(accB_node, W, fB);
}

void Squirrel::remove_piece_feature(int piece, int sq)
{
    int fW = piece * 64 + sq;
    lin::vsmi(accW_node, W, fW);

    int fB = mirror_piece(piece) * 64 + mirror_sq(sq);
    lin::vsmi(accB_node, W, fB);
}

// void Squirrel::add_global_feature(int fid)
// {
//     lin::vami(accW_node, W, fid);
//     lin::vami(accB_node, W, fid);
// }

// void Squirrel::remove_global_feature(int fid)
// {
//     lin::vsmi(accW_node, W, fid);
//     lin::vsmi(accB_node, W, fid);
// }



Squirrel::Squirrel(const std::string& W_path, const Board& root_pos)
{
    root = root_pos;
    W = lin::loadm(wPath, hW);

    auto bind = [](lin::vec& v, float* buf) {
        v.data     = buf;
        v.shape[0] = FEAT_DIM;
        v.shape[1] = 1;
    };

    Allocate output buffers
    float out_buf[rows];       // size == rows
    lin::vec out{ out_buf, { rows, 1 } };


    accW_node = {bufW_node, {836, 1}};
    accB_node = {bufB_node, {836, 1}};
    accW_root = {bufW_root, {836, 1}};
    accB_root = {bufB_root, {836, 1}};

    // Fill root accumulators
    for (int p = 0; p < 12; ++p) {
        U64 bb = root.bitboards[p];
        while (bb) {
            int sq = std::countr_zero(bb);
            bb &= bb - 1;
            // add_piece_feature for root
            lin::vami(accW_root, W, piece * 64 + sq);
            lin::vami(accB_root, W, mirror_piece(piece) * 64 + mirror_sq(sq));
        }
    }
    // TODO: init castling rights
    // TODO: init en-passant

    reset();
}

void Squirrel::make(const Move& m)
{
    const int from  = m.from();
    const int to    = m.to();
    const U64 mask_from = square(from);
    const U64 mask_to = square(to);
    const int flags = m.flags();
    const bool side = node.side_to_move;
    node.side_to_move = !side;

    // Locate moving piece
    int moving = -1;
    for (int p = side ? 0 : 6; p < (side ? 6 : 12); ++p)
        if (node.bitboards[p] & mask_from) {
            moving = p;
            node.bitboards[p] &= ~mask_from;
            remove_piece_feature(p, from);
            break;
        }

    // Handle captures on target
    for (int p = side ? 6 : 0; p < (side ? 12 : 6); ++p)
        if (node.bitboards[p] & mask_to) {
            node.bitboards[p] &= ~mask_to;
            remove_piece_feature(p, to);
            node.halfmove_clock = 0;
            // rook captured on initial square → lose opponent castling right
            if (p == (side ? 9 : 3)) {
                if (to == (side ? 56 : 0))
                    node.castling_rights &= side ? ~0b1000 : ~0b0010;
                else if (to == (side ? 63 : 7))
                    node.castling_rights &= side ? ~0b0100 : ~0b0001;
            }
            break;
        }

    // Pawn specifics
    bool pown_move = (moving == 0 || moving == 6);
    if (pown_move) node.halfmove_clock = 0;

    // En-passant capture
    if (pown_move && to == node.en_passant_square) {
        int cap_sq = side ? to - 8 : to + 8;
        node.bitboards[side ? 6 : 0] &= ~square(cap_sq);
        remove_piece_feature(side ? 6 : 0, cap_sq);
    }

    // Promotion
    if (pown_move && flags >= 4 && flags <= 7) {
        moving = side ? PROMO_WHITE[flags - 4] : PROMO_BLACK[flags - 4];
    }

    // Clear/set en-passant square
    // TODO: upd en-passant feature 
    node.en_passant_square = -1;
    if (pown_move && std::abs(to - from) == 16) {
        node.en_passant_square = side ? from + 8 : from - 8;
    }

    // Update castling rights for mover
    // TODO: upd castling feature 
    uint8_t mask = 0xFF;
    if (moving == (side ? 5 : 11)) {            
        // king moved
        mask ^= side ? 0b0011 : 0b1100;
    } else if (moving == (side ? 3 : 9)) {       
        // rook moved
        if (from == (side ? 0 : 56))
            mask ^= side ? 0b0010 : 0b1000;
        else if (from == (side ? 7 : 63))
            mask ^= side ? 0b0001 : 0b0100;
    }
    node.castling_rights &= mask;

    // Handle castling move itself
    if (moving == (side ? 5 : 11) && std::abs(to - from) == 2) {
        node.bitboards[side ? 3 : 9] ^= node.CASTLING_MASKS[(to < from) * 2 + side];
    }

    // Place moving piece on target
    node.bitboards[moving] |= mask_to;
    add_piece_feature(moving, to);

    // Halfmove / fullmove counters
    ++node.halfmove_clock;
    if (!side) ++node.fullmove_number;

}



void Squirrel::reset()
{
    // Restore board to the root position
    node = root;

    // Copy of root accumulators into working ones
    std::copy(accW_root.data, accW_root.data + accW_root.shape[0], accW_node.data);
    std::copy(accB_root.data, accB_root.data + accB_root.shape[0], accB_node.data);
}



void Squirrel::jump(const Move& m) 
{
    make(m);

    root = node;
    std::copy(bufW_node, bufW_node + 836, bufW_root);
    std::copy(bufB_node, bufB_node + 836, bufB_node);
}