#include "Tree.hpp"
#include <cmath>
#include <algorithm>
#include <random>

Tree::Tree(Squirrel& sqr_, std::size_t lim, float cp)
    : sqr(sqr_), nodes_lim(lim), c_puct(cp), rng(std::random_device{}()) {}

// build a brand-new tree with given root position
void Tree::new_game(const Board& root_pos)
{
    nodes.clear();
    nodes.reserve(1000000);
    root_idx = 0;
    nodes.emplace_back();           // create root node

    // initialise network for this position
    sqr.set_root(root_pos);
    sqr.eval();

    Node& root = nodes[root_idx];
    root.terminal     = sqr.terminal;
    root.visits       = 0;
    root.total_value  = 0.0f;

    if (!root.terminal) {
        root.moves = sqr.moves;
        softmax(sqr.policy_logits_, root.priors);
        root.children.assign(root.moves.size(), -1);
    }
}

void Tree::softmax(const std::vector<float>& logits, std::vector<float>& probs)
{
    float max_log = *std::max_element(logits.begin(), logits.end());
    float sum = 0.0f;
    probs.resize(logits.size());

    for (int i = 0; i < logits.size(); ++i) {
        float e = std::exp(logits[i] - max_log);
        probs[i] = e;
        sum += e;
    }
    float inv = 1.0f / sum;
    for (float& p : probs) 
        p *= inv;
}

int Tree::select_child(int node_idx) const
{
    const Node& node = nodes[node_idx];
    float sqrt_n = std::sqrt(float(node.visits + 1));
    float best_score = -1e9f;
    int   best_id = -1;

    assert(node.moves.size() > 0);

    for (int i = 0; i < node.moves.size(); ++i) {
        int child = node.children[i];
        float q = 0.0f;
        int   n = 0;
        if (child != -1 && nodes[child].visits) {
            n = nodes[child].visits;
            q = nodes[child].total_value / n;
        }
        float score = q + c_puct * node.priors[i] * sqrt_n / (1.0f + n);
        if (score > best_score) {
            best_score = score;
            best_id = i;
        }
    }
    return best_id;
}


int Tree::expand_node(int parent_idx, int move_idx)
{
    Move move = nodes[parent_idx].moves[move_idx];

    // apply move to squirrel
    sqr.make(move);
    sqr.eval();

    // create new node
    int child_idx = nodes.size();
    nodes.emplace_back();
    Node& child = nodes.back();

    child.terminal    = sqr.terminal;
    child.visits      = 0;
    child.total_value = 0.0f;
    child.leaf_eval   = sqr.value_;

    if (!child.terminal) {
        child.moves = sqr.moves;
        softmax(sqr.policy_logits_, child.priors);
        child.children.assign(child.moves.size(), -1);
    }

    // link child to parent
    nodes[parent_idx].children[move_idx] = child_idx;

    return child_idx;
}


void Tree::playout()
{
    std::vector<int> path;
    int node_idx = root_idx;

    sqr.reset();  // go back to root position

    while (true) {
        Node& node = nodes[node_idx];
        path.push_back(node_idx);
        node.visits++;

        if (node.terminal) {
            node_log[2]++;
            backup(path, node.leaf_eval); // terminal node reached
            return;
        }

        int move_idx = select_child(node_idx);
        assert(move_idx != -1);
        int child_idx = node.children[move_idx];

        if (child_idx == -1) {
            // expansion
            node_log[1]++;
            child_idx = expand_node(node_idx, move_idx);
            path.push_back(child_idx);
            nodes[child_idx].visits++;
            backup(path, sqr.value_);
            return;
        } else {
            // follow to child
            node_log[0]++;
            node_idx = child_idx;
            const Move& move = node.moves[move_idx];
            sqr.make(move);
        }
    }
}

void Tree::backup(const std::vector<int>& path, float leaf_value)
{
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        Node& node = nodes[*it];
        node.total_value += leaf_value;
        leaf_value = -leaf_value;  // side to move changes after each ply
    }
}


int Tree::best_child(bool use_temperature, float tau) const
{
    const Node& root = nodes[root_idx];
    int n = root.moves.size();

    if (n == 0) return -1;

    std::vector<float> scores(n);

    for (int i = 0; i < n; ++i) {
        int child = root.children[i];
        int visits = (child != -1) ? nodes[child].visits : 0;
        scores[i] = float(visits);
    }

    if (!use_temperature || tau == 0.0f) {
        int best = int(std::max_element(scores.begin(), scores.end()) - scores.begin());
        return best;
    } else {
        float inv_tau = 1.0f / tau;
        float sum = 0.0f;
        for (float& sc : scores) {
            sc = std::pow(sc, inv_tau);
            sum += sc;
        }
        if (sum == 0.0f) {
            std::uniform_int_distribution<int> dist(0, n-1);
            return dist(rng);
        }
        std::uniform_real_distribution<float> dist(0.0f, sum);
        float r = dist(rng);

        for (int i = 0; i < n; ++i) {
            r -= scores[i];
            if (r <= 0.0f)
                return i;
        }
        return n - 1; // fallback
    }
}


Move Tree::best_move(bool use_temperature, float tau) const
{
    int idx = best_child(use_temperature, tau);
    if (idx == -1) return Move(); // fallback dummy move
    return nodes[root_idx].moves[idx];
}


void Tree::step(int child_idx)
{
    if (child_idx == -1) {
        std::cout << "No legal moves – game over\n";
    } else {
        sqr.reset();
        sqr.jump(nodes[root_idx].moves[child_idx]);

        root_idx = nodes[root_idx].children[child_idx];
        assert(root_idx != -1);
    }
}


// void Tree::change_root(int child_idx)
// {
    // if (nodes.size() <= nodes_lim) {
    // root_idx = nodes[root_idx].children[child_idx];
    // } else {
    //     new_game(new_root_board);
    // }
// }