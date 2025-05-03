#pragma once

#include "Board.hpp"
#include "Move.hpp"
#include "Squirrel.hpp"

#include <vector>
#include <cstddef>
#include <random>

class Tree {
public:
    struct Node {
        std::vector<Move>  moves;        // legal moves from this position
        std::vector<float> priors;       // P(s,a) after softmax
        std::vector<int>   children;     // indices in nodes[], -1 if not expanded
        int   visits      = 0;           // N(s)
        float total_value = 0.0f;        // W(s)
        bool  terminal    = false;
        float leaf_eval   = 0.0f;
    };

    Tree(Squirrel& sqr,
         std::size_t nodes_lim = 1000000,
         float c_puct          = 1.5f);

    void new_game(const Board& root_pos); // create new tree from given root position
    void playout(); // run simulation
    Move best_move(bool use_temperature, float tau = 1.0f) const; // pick best move 
    int best_child(bool use_temperature, float tau = 1.0f) const;
    void step(int child_idx);

    std::vector<Node>   nodes;
    int node_log[3] = {0, 0, 0}; // Following, Expansion, Terminal

    int root_idx = 0;

    Squirrel& sqr;

private:
    // select child index of node using PUCT
    int select_child(int node_idx) const;

    // expand leaf
    int expand_node(int parent_idx, int move_idx);

    // backup value
    void backup(const std::vector<int>& path, float leaf_value);

    // stable softmax for logits → probabilities
    static void softmax(const std::vector<float>& logits, std::vector<float>& probs);

    // data
    std::size_t         nodes_lim;
    float               c_puct;
    mutable std::mt19937 rng;
};
