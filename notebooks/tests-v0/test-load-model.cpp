#include <iostream>
#include <format>
#include "Board.hpp"
#include "Move.hpp"
#include "Squirrel.hpp"

int main()
{
    const std::string weight_dir = "data";

    Board root;

    Squirrel sqrl(weight_dir);
    sqrl.set_root(root);

    Move();

    // Evaluate the starting position
    sqrl.eval();
    std::cout << std::format("Initial value: {:.2f}\n", sqrl.value());
    std::cout << "Initial legal moves: " << sqrl.moves.size() << '\n';

    // Show first five policy logits
    for (int i = 0; i < sqrl.moves.size(); ++i)
        std::cout << std::format("{}:{:.3f}, ", sqrl.moves[i].uci(), sqrl.policy()[i]);
    std::cout << std::endl;

    // Play e2e4 d7d5
    sqrl.make(Move::from_uci("e2e4"));
    sqrl.make(Move::from_uci("d7d5"));
    sqrl.make(Move::from_uci("g1f3"));

    // Re-evaluate the new position
    sqrl.eval();
    std::cout << std::format("\nAfter e2e4 d7d5 g1f3 value: {:.2f}\n", sqrl.value());
    std::cout << "Moves in this position: " << sqrl.moves.size() << '\n';

    for (int i = 0; i < sqrl.moves.size(); ++i)
        std::cout << std::format("{}:{:.3f}, ", sqrl.moves[i].uci(), sqrl.policy()[i]);
    std::cout << std::endl;

    // Reset back to the starting position and verify value restored
    sqrl.reset();
    sqrl.eval();
    std::cout << std::format("\nAfter reset  value: {:.2f}\n", sqrl.value());

    return 0;
}
