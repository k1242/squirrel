#include "lin.hpp"  

class Squirrel {
public:
    Board    node;
    lin::vec accW_node;
    lin::vec accB_node;

    Board    root;
    lin::vec accW_root;
    lin::vec accB_root;

    Squirrel(const lin::mat& W, const Board& rootPos);

    void make(const Move& m);
    void jump(const Move& m)
    void reset();

private:
    cnpy::NpyArray hM;
    const lin::mat& W;

    void add_feature   (int piece, int sq);
    void remove_feature(int piece, int sq);

    void add_piece_feature   (int piece, int sq);
    void remove_piece_feature(int piece, int sq);
    // void add_global_feature  (int fid);
    // void remove_global_feature(int fid);

    float bufW_node[836]{};
    float bufB_node[836]{};
    float bufW_root[836]{};
    float bufB_node[836]{};

};
