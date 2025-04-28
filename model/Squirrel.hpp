#pragma once

#include "Board.hpp"
#include "Move.hpp"
#include "Linear.hpp"
#include "lin.hpp"
#include <vector>
#include <string>

class Squirrel {
public:
    // root and node boards
    Board root;
    Board node;

    // Construct network and initialize root position
    explicit Squirrel(const std::string& weight_dir, const Board& root_pos);

    void make(const Move& m); // play a move
    void jump(const Move& m); // play a move and treat the new position as a root
    void reset(); // restore the position to the original root

    // Run Body → Critic → Actor, fill value_ and policy_logits_
    void evaluate();

    // Accessors
    float value() const                     { return value_; }
    const std::vector<float>& policy() const{ return policy_logits_; }
    bool  is_white() const                  { return node.side_to_move; }

    std::vector<Move> moves; // legal moves of the current node, filled by evaluate()

private:
    // helpers
    inline float acc(bool side, int i) const
    { return side ? accW_node[i] : accB_node[i]; }

    // evaluation stages
    void encode_body();
    void run_critic();
    void run_actor();

    // network weights
    static constexpr int DIM = 256;
    Linear B1p, B1o, B2, C1, A1, A2;
    lin::mat A0w;

    // working buffers
    float buf_xo[DIM]{};
    float buf_xp[DIM]{};
    float buf_xj[64]{};
    float buf_xm[32]{};

    lin::vec xo{buf_xo,{DIM,1}}, xp{buf_xp,{DIM,1}}, xj{buf_xj,{64,1}}, xm{buf_xm,{32,1}};

    // results
    float value_{0.0f};
    std::vector<float> policy_logits_;

    // accumulators and board state
    static constexpr int DIM_ACC = 256;
    float bufW_node[DIM_ACC]{}, bufB_node[DIM_ACC]{};
    float bufW_root[DIM_ACC]{}, bufB_root[DIM_ACC]{};

    lin::vec accW_node{bufW_node,{DIM_ACC,1}}, accB_node{bufB_node,{DIM_ACC,1}};
    lin::vec accW_root{bufW_root,{DIM_ACC,1}}, accB_root{bufB_root,{DIM_ACC,1}};

    // B0 embedding weights
    lin::mat B0w;
    lin::vec B0a;
    cnpy::NpyArray hB0w, hB0a, hA0w;

    // internal helpers for piece updates
    void add_piece_feature(int piece, int sq);
    void remove_piece_feature(int piece, int sq);
};
