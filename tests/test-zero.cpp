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