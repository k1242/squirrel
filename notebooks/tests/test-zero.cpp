#include <iostream>
#include <array>
#include <chrono>
#include <cstdint>

using U64 = uint64_t;
constexpr int N = 12;
constexpr int ITERATIONS = 10'000'000;

void test_std_array() {
    std::array<U64, N> a{};
    std::array<U64, N> b{};

    for (int i = 0; i < N; ++i)
        a[i] = i;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; ++i)
        b = a;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "std::array copy time: " << diff.count() << " s\n";
}

void test_c_array() {
    U64 a[N]{};
    U64 b[N]{};

    for (int i = 0; i < N; ++i)
        a[i] = i;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; ++i)
        for (int j = 0; j < N; ++j)
            b[j] = a[j];

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "C-array copy time: " << diff.count() << " s\n";
}

int main() {
    test_std_array();
    test_c_array();
    return 0;
}





#include <iostream>
#include "Board.hpp"
#include "Move.hpp"
#include "notation.hpp"
#include "movegen.hpp"
#include <cstdint>

int main() {
    std::cout << std::endl;

    // Board board;

    // fen en_passant black
    // Board board("rnbqkbnr/1ppppppp/p7/4P3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2");
    // board = Board(board, Move::from_uci("h7h5")); // here 
    // board = Board(board, Move::from_uci("d7d5"));  // not here
    
    // fen en_passant white
    // Board board("r1bqkb1r/N1ppp2p/2n3pn/6N1/5pP1/8/PPPPPP1P/R1BQKBR1 b Qkq g3 0 9");

    // en_passant 
    // Board board("3q1br1/r3k2p/n2p1n2/pPpPpp2/1P3Np1/2BK4/PR1N1P1P/2Q1RB2 w - - 10 34");
    // board = Board(board, Move::from_uci("h2h4"));
    // board = Board(board, Move::from_uci("g4h3"));

    // castling
    // Board board("r3k2r/pppRqp1p/1n1p1np1/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R3K2R b KQkq - 0 10");
    // board = Board(board, Move::from_uci("e1g1")); // w: O-O
    // board = Board(board, Move::from_uci("e1c1")); // w: O-O-O
    // board = Board(board, Move::from_uci("e8g8")); // b: O-O
    // board = Board(board, Move::from_uci("e8c8")); // b: O-O-O

    // kill the king rook
    // Board board("4kb1r/2rb1p2/1pn1p1Bp/p1p2q2/P5P1/BP2P2N/1Q1PKP1P/RN2R3 w k - 2 20");
    // board = Board(board, Move::from_uci("b2h8"));

    // Board board;
    // board = Board(board, Move::from_uci("e2e4"));
    // board = Board(board, Move::from_uci("e7e5"));
    // board = Board(board, Move::from_uci("d1f3"));

    // Board board("n1k5/1pppp1P1/8/3R4/8/5P2/pPPPP2P/1NBQKBNR b - - 10 1");
    Board board("5rk1/5pp1/7B/4N3/2B3Q1/8/8/4K2R b K - 0 1");

    // auto occupancies = get_occupancies(board);

    // U64 attacked = 0ULL;
    // for (int sq = 0; sq < 64; ++sq) {
    //     if (is_square_attacked(board, occupancies, sq, board.side_to_move)) attacked |= square(sq);
    // }

    std::vector<Move> moves = board.legal_moves();

    // generate_pawn_moves(board, occupancies, moves);
    // generate_knight_moves(board, occupancies, moves);
    // generate_bishop_moves(board, occupancies, moves);
    // generate_rook_moves(board, occupancies, moves);
    // generate_queen_moves(board, occupancies, moves);
    // generate_king_moves(board, occupancies, moves);

    print_board(board);
    // print_bitboard(attacked);

    std::cout << "# moves: " << moves.size() << std::endl;
    print_moves(moves);
    // std::cout << (square(59) | square(58) | square(57)) << std::endl;

    std::cout << std::endl;
    return 0;
}
// 14ULL
// 1008806316530991104ULL










Можешь опираться на реализацию из bbc, но учти что мы работаем в C++ и у меня часть функций отличаются
```
// leaf nodes (number of positions reached during the test of the move generator at a given depth)
U64 nodes;

// perft driver
static inline void perft_driver(int depth)
{
    // reccursion escape condition
    if (depth == 0)
    {
        // increment nodes count (count reached positions)
        nodes++;
        return;
    }
    
    // create move list instance
    moves move_list[1];
    
    // generate moves
    generate_moves(move_list);
    
        // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {   
        // preserve board state
        copy_board();
        
        // make move
        if (!make_move(move_list->moves[move_count], all_moves))
            // skip to the next move
            continue;
        
        // call perft driver recursively
        perft_driver(depth - 1);
        
        // take back
        take_back();        
    }
}

// perft test
void perft_test(int depth)
{
    printf("\n     Performance test\n\n");
    
    // create move list instance
    moves move_list[1];
    
    // generate moves
    generate_moves(move_list);
    
    // init start time
    long start = get_time_ms();
    
    // loop over generated moves
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {   
        // preserve board state
        copy_board();
        
        // make move
        if (!make_move(move_list->moves[move_count], all_moves))
            // skip to the next move
            continue;
        
        // cummulative nodes
        long cummulative_nodes = nodes;
        
        // call perft driver recursively
        perft_driver(depth - 1);
        
        // old nodes
        long old_nodes = nodes - cummulative_nodes;
        
        // take back
        take_back();
        
        // print move
        printf("     move: %s%s%c  nodes: %ld\n", square_to_coordinates[get_move_source(move_list->moves[move_count])],
                                                  square_to_coordinates[get_move_target(move_list->moves[move_count])],
                                                  get_move_promoted(move_list->moves[move_count]) ? promoted_pieces[get_move_promoted(move_list->moves[move_count])] : ' ',
                                                  old_nodes);
    }
    
    // print results
    printf("\n    Depth: %d\n", depth);
    printf("    Nodes: %lld\n", nodes);
    printf("     Time: %ld\n\n", get_time_ms() - start);
}
```




// bool Board::make_move(const Move& m, Undo& u) {
//     u.move = m;
//     u.captured_piece = -1;
//     u.castling_rights = castling_rights;
//     u.en_passant_square = en_passant_square;
//     u.halfmove_clock = halfmove_clock;

//     int from = m.from(), to = m.to(), flags = m.flags();
//     bool white = side_to_move;
//     int piece = -1;

//     for (int p = white ? 0 : 6; p < (white ? 6 : 12); ++p)
//         if (bitboards[p] & square(from)) { piece = p; break; }

//     bitboards[piece] &= ~square(from);

//     for (int p = white ? 6 : 0; p < (white ? 12 : 6); ++p)
//         if (bitboards[p] & square(to)) {
//             bitboards[p] &= ~square(to);
//             u.captured_piece = p;
//             halfmove_clock = 0;
//             break;
//         }

//     if ((piece == 0 || piece == 6) && to == en_passant_square) {
//         int capSq = white ? to - 8 : to + 8;
//         bitboards[white ? 6 : 0] &= ~square(capSq);
//         u.captured_piece = white ? 6 : 0;
//         halfmove_clock = 0;
//     }

//     en_passant_square = -1;
//     if (piece == 0 || piece == 6) {
//         halfmove_clock = 0;
//         if (abs(to - from) == 16)
//             en_passant_square = white ? from + 8 : from - 8;
//         if (flags >= 4 && flags <= 7)
//             piece = white ? PROMO_WHITE[flags - 4] : PROMO_BLACK[flags - 4];
//     }

//     if (piece == (white ? 5 : 11) && abs(to - from) == 2)
//         bitboards[white ? 3 : 9] ^= CASTLING_MASKS[(to < from)*2 + white];

//     bitboards[piece] |= square(to);

//     uint8_t mask = 0xFF;
//     if (piece == (white ? 5 : 11))
//         mask ^= (white ? 0b0011 : 0b1100);
//     else if (piece == (white ? 3 : 9)) {
//         if (from == (white ? 0 : 56))      mask ^= (white ? 0b0010 : 0b1000);
//         else if (from == (white ? 7 : 63)) mask ^= (white ? 0b0001 : 0b0100);
//     }
//     castling_rights &= mask;

//     ++halfmove_clock;
//     if (!white) ++fullmove_number;
//     side_to_move = !side_to_move;

//     auto occ = get_occupancies(*this);
//     if (is_king_attacked(*this, occ, 1 - side_to_move)) {
//         unmake_move(u);
//         return false;
//     }
//     return true;
// }

// void Board::unmake_move(const Undo& u) {
//     int from = u.move.from(), to = u.move.to(), flags = u.move.flags();
//     side_to_move = !side_to_move;
//     bool white = side_to_move;
//     int piece = -1;

//     for (int p = white ? 0 : 6; p < (white ? 6 : 12); ++p)
//         if (bitboards[p] & square(to)) { piece = p; break; }

//     bitboards[piece] &= ~square(to);
//     if ((piece == 4 || piece == 10) && (flags >= 4 && flags <= 7))
//         piece = white ? 0 : 6;

//     bitboards[piece] |= square(from);

//     if (u.captured_piece != -1) {
//         int sq = (piece == 0 || piece == 6) && to == en_passant_square
//                  ? (white ? to - 8 : to + 8) : to;
//         bitboards[u.captured_piece] |= square(sq);
//     }

//     if (piece == (white ? 5 : 11) && abs(to - from) == 2)
//         bitboards[white ? 3 : 9] ^= CASTLING_MASKS[(to < from)*2 + white];

//     castling_rights = u.castling_rights;
//     en_passant_square = u.en_passant_square;
//     halfmove_clock = u.halfmove_clock;
//     if (white) --fullmove_number;
// }



// std::vector<Move> Board::legal_moves() {
//     std::vector<Move> pseudo;
//     auto occ = get_occupancies(*this);
//     generate_pawn_moves(*this, occ, pseudo);
//     generate_knight_moves(*this, occ, pseudo);
//     generate_bishop_moves(*this, occ, pseudo);
//     generate_rook_moves(*this, occ, pseudo);
//     generate_queen_moves(*this, occ, pseudo);
//     generate_king_moves(*this, occ, pseudo);

//     std::vector<Move> legal;
//     Undo u;
//     for (const Move& m : pseudo)
//         if (make_move(m, u)) { 
//             legal.push_back(m); 
//             unmake_move(u); 
//         }
//     return legal;
// }



:: g++ -std=c++20 -Iengine -Itests tests/test-move.cpp engine/Move.cpp engine/notation.cpp -o test
:: g++ -std=c++20 -Iengine -Itests tests/test-me.cpp engine/Move.cpp engine/Board.cpp engine/notation.cpp -o test

:: g++ -std=c++20 -Itests tests/test-zero.cpp -o test

:: g++ -std=c++20 -Ofast -Iengine -Itests tests/test-board.cpp engine/Move.cpp engine/Board.cpp engine/notation.cpp engine/movegen.cpp -o test




:: TODO: добавить обработку корректности взятия на проходе

:: TODO: добавить флаги на хитрые ходы, делать ленивую проверку без полноценной генерации

:: g++ -std=c++20 -Ofast -Itests tests/test-simd.cpp -o test -march=native

:: 3ms for 4k x 4k float, 6ms double

:: build/Board.o build/lin.o build/Move.o build/movegen.o build/notation.o build/test.o

:: g++ -std=c++20 -O2 -o test ^
::  -Icnpy -Ilin ^
::  tests/test-load.cpp cnpy/cnpy.cpp lin/lin.cpp

:: g++ -std=c++20 -Ofast -march=native tests/test-matvec.cpp -o test



:: python tests/test-save.py




// U64 perft_squirrel(Board& board, int depth, Squirrel& squirrel) {
//     if (depth == 0) return 1;

//     U64 nodes = 0ULL;
//     std::vector<Move> moves = board.legal_moves();

//     for (const Move& m : moves) {
//         Board next(board, m);

//         squirrel.node = board;
//         squirrel.make(m);
//         if (next != squirrel.node) std::cout << "err\n";

//         nodes += perft_squirrel(next, depth - 1, squirrel);
//     }
//     return nodes;
// }


// ---------------------------- TEST LOAD ---------------------------------


#include <iostream>
#include <chrono>
#include <format>
#include "Board.hpp"
#include "Linear.hpp"
#include "notation.hpp"
#include "Squirrel.hpp"
#include "Board.hpp"
#include "Move.hpp"
#include "lin.hpp"  
#include "bit"



int main() {

    int DIM = 256;

    Linear B1p("data/B1p-32-256-w.npy", "data/B1p-32-a.npy");
    Linear B1o("data/B1o-32-256-w.npy", "data/B1o-32-a.npy");
    Linear B2("data/B2-32-64-w.npy", "data/B2-32-a.npy");
    Linear C1("data/C1-1-32-w.npy", "data/C1-1-a.npy");
    Linear A1("data/A1-32-32-w.npy", "data/A1-32-a.npy");
    Linear A2("data/A2-1-32-w.npy", "data/A2-1-a.npy");



    float hxo[DIM]; 
    float hxp[DIM];
    float hxj[64];
    float hxm[32];
    float res;
    lin::vec xo = {hxo, {DIM,1}};
    lin::vec xp = {hxp, {DIM,1}};
    lin::vec xj = {hxj, {64,1}};
    lin::vec xm = {hxm, {32,1}};

    Board root;
    Squirrel sqr("data/B0-768-256-w.npy", "data/B0-256-a.npy", root);
    
    sqr.make(Move::from_uci("e2e4"));
    sqr.make(Move::from_uci("d7d5"));



    int side = sqr.is_white();

    // relu
    for (int i = 0; i < DIM; ++i) {
        xo[i] = (sqr.acc(side, i) < 0) ? 0.0f : sqr.acc(side, i);
        xp[i] = (sqr.acc(side^1, i) < 0) ? 0.0f : sqr.acc(side^1, i);
    }

    B1p(xp); B1p.relu();
    B1o(xp); B1o.relu();

    for (int i = 0; i < 32; ++i) {
        xj[i] = B1p.inner[i];
        xj[i+32] = B1o.inner[i];
    }

    B2(xj); B2.relu();

    C1(B2.inner);

    res = C1.inner[0];

    std::cout << std::format("res = {:.2f}\n", res);

    

    // ---------- encode move ----------

    cnpy::NpyArray hA0w;
    lin::mat A0w;
    A0w = lin::loadm("data/A0-768-32-w.npy", hA0w);

    int from;
    U64 mask_from;

    std::vector<Move> moves = sqr.node.legal_moves();
    std::vector<float> policy_logits(moves.size()); 

    auto m = moves[0];

    from = m.from();
    mask_from = square(from);
    int moving = -1;
    for (int p = side ? 0 : 6; p < (side ? 6 : 12); ++p)
        if (sqr.node.bitboards[p] & mask_from) {
            moving = p;
            break;
        }
    std::cout << std::format("{} : {} → {}\n", moving, moves[0].from(), moves[0].to());

    A1(B2.inner);

    for (int i = 0; i < xm.shape[0]; ++i)
        xm[i] = 0.0f;

    lin::vsmi(xm, A0w, moving * 64 + from);
    lin::vami(xm, A0w, moving * 64 + m.to());

    lin::vavi(xm, A1.inner);

    // relu
    for (int i = 0; i < xm.shape[0]; ++i)
        xm[i] = (xm[i] < 0.0f) ? 0.0f : xm[i];

    A2(xm);

    // print_vec(xm, -1);

    policy_logits[0] = A2.inner[0];

    std::cout << std::format("({}, {}, {}, ...)\n", policy_logits[0], policy_logits[1], policy_logits[2]);



    // std::cout << std::format("xj.shape = ({},)\n", xj.shape[0]);
    // std::cout << std::format("fc2.shape = ({}, {})\n", fc2.W.shape[0], fc2.W.shape[1]);

    // print_vec(fc1p.inner, 10);
    // print_vec(fc1o.inner, 10);
    // print_vec(xj, 10);
    // print_vec(fc1p.inner, -1);


    // float hx[256]{};
    // lin::vec h = {hx, {256,1}};



    // std::cout << std::format("W.shape = ({}, {})\n", squirrel.accW.shape[0], squirrel.accW.shape[1]);
    // std::cout << std::format("W.shape = ({}, {})\n", fc0.W.shape[0], fc0.W.shape[1]);
    // std::cout << std::format("A.shape = ({}, {})\n", fc0.A.shape[0], fc0.A.shape[1]);

    // fc1(squirrel.accW); fc1.relu();

    // print_vec(fc1.inner, -1);

    // fc2(fc1.inner);

    // std::cout << std::format("{}\n", fc2.inner[0]);


    // lin::print_vec(squirrel.accW, 5);
    // squirrel.make(Move::from_uci("e2e4"));
    // squirrel.make(Move::from_uci("e7e5"));
    // lin::print_vec(squirrel.accW, 5);
    // squirrel.reset();
    // lin::print_vec(squirrel.accW, 5);


    // Board root2;
    // root2 = Board(root2, Move::from_uci("d2d4"));
    // root2 = Board(root2, Move::from_uci("e7e5"));
    // root2 = Board(root2, Move::from_uci("b1c3"));
    // root2 = Board(root2, Move::from_uci("g8f6"));
    // // print_board(root2);
    // Squirrel squirrel2("data/W256.npy", root2);

    // float hdelta[256]{};
    // lin::vec delta = {hdelta, {256,1}};

    // lin::vsvc(squirrel1.accW_root, squirrel2.accB_root, delta);

    // std::cout << std::format("L1(1W, 2B) = {:.2f}\n", lin::L1(delta));

    // lin::vsvc(squirrel1.accB_root, squirrel2.accW_root, delta);

    // std::cout << std::format("L1(1B, 2W) = {:.2f}\n", lin::L1(delta));

    // lin::vsvc(squirrel1.accB_root, squirrel1.accW_root, delta);

    // std::cout << std::format("L1(1B, 1W) = {:.2f}\n", lin::L1(delta));

    // lin::vsvc(squirrel2.accB_root, squirrel2.accW_root, delta);

    // std::cout << std::format("L1(2B, 2W) = {:.2f}\n", lin::L1(delta));



    // lin::print_vec(delta);
    // lin::print_vec(squirrel1.accW_root);
    // lin::print_vec(squirrel2.accW_root);

    // Board root_copy;
    // Board root("rr6/8/8/8/8/8/8/8 w - - 0 1");

    // Move moves[4] = {Move::from_uci("e2e4"), Move::from_uci("e7e5"), Move::from_uci("d1f3"), Move::from_uci("b8c6")};

    // for (auto m : moves) root_copy = Board(root_copy, m);
    // print_board(root_copy);

    // Squirrel squirrel("data/W2.npy", root);
    // for (auto m : moves) squirrel.make(m);
    // print_board(squirrel.node);

    // std::cout << (root_copy == squirrel.node) << std::endl;

    // print_vec(squirrel.accW_node);

    // Squirrel squirrel("data/W256.npy", root);

    // int depth = 5;
    // auto start = std::chrono::high_resolution_clock::now();
    // U64 nodes = perft_squirrel(root, depth, squirrel);
    // auto stop  = std::chrono::high_resolution_clock::now();
    // double ms = std::chrono::duration<double, std::milli>(stop - start).count();

    // std::cout << "\nDepth " << depth << " nodes: " << nodes << '\n';
    // std::cout << "Time:          " << ms/1000.0   << " s\n";
    // std::cout << "NPS:           " << (nodes / (ms / 1000.0)) << '\n';


    // print_board(board);

    // Linear layer("data/M0.npy", "data/a0.npy");


    // float x_buf[5] = {1, 0, 0, 0, 0};
    // lin::vec x{x_buf, {5, 1}};
    // float y_buf[3] = {1.5, 2, -1};
    // lin::vec y{y_buf, {3, 1}};
    // lin::print_vec(x);
    // layer(x); relu(x);
    // lin::print_vec(x);
    // lin::vavi(x, y);
    // lin::print_vec(x);
    
    // cnpy::NpyArray hM, hv;
    // lin::mat M = lin::loadm("data/M0.npy", hM);
    // lin::vec v = lin::loadv("data/a0.npy", hv);

    // int rows = M.shape[0];
    // int cols = M.shape[1];

    // // Display loaded data
    // std::cout << "Matrix M (" << rows << "x" << cols << "):\n";
    // for (int i = 0; i < rows; ++i) {
    //     for (int j = 0; j < cols; ++j)
    //         std::cout << M[i * cols + j] << " ";
    //     std::cout << "\n";
    // }
    // std::cout << "\nVector v:\n";
    // for (int i = 0; i < v.shape[0]; ++i) std::cout << v[i] << ' ';
    // std::cout << "\n\n";

    // Allocate output buffers
    // float out_buf[rows];       // size == rows
    // lin::vec out{ out_buf, { rows, 1 } };

    // 1) out = M @ v  (vmmc)
    // lin::vmmc(v, M, out);
    // std::cout << "out = M @ v:\n";
    // for (int i = 0; i < out.shape[0]; ++i) std::cout << out[i] << ' ';
    // std::cout << "\n\n";

    // 2) v += out  (vavi)
    // lin::vavi(v, v);
    // std::cout << "v after v += v:\n";
    // for (int i = 0; i < v.shape[0]; ++i) std::cout << v[i] << ' ';
    // std::cout << "\n\n";

    // 3) v -= out  (vsvi) — restore original v
    // lin::vsvi(v, out);

    // 4) v += first row of M  (vami)
    // lin::vami(v, M, 0);
    // std::cout << "v after adding 0 row of M:\n";
    // for (int i = 0; i < v.shape[0]; ++i) std::cout << v[i] << ' ';
    // std::cout << "\n\n";

    // 5) v -= first row of M  (vsmi) — restore again
    // lin::vsmi(v, M, 1);
    // std::cout << "v after subtracting 1 row of M (original again):\n";
    // for (int i = 0; i < v.shape[0]; ++i) std::cout << v[i] << ' ';
    // std::cout << '\n';

    return 0;
}
