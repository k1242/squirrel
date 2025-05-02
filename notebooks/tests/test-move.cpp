#include <iostream>
#include "Move.hpp"
#include "notation.hpp"


int main() {
    std::cout << std::endl;
    // Create moves from squares and flags
    Move m1(square_index("e2"), square_index("e4"));
    Move m2(square_index("e7"), square_index("e8"), promotion_flag('q'));

    std::cout << "Move 1: " << m1.uci() << '\n';
    std::cout << "  From: " << square_name(m1.from()) << '\n';
    std::cout << "  To:   " << square_name(m1.to()) << '\n';
    std::cout << "  Flags: " << int(m1.flags()) << '\n';

    std::cout << "Move 2: " << m2.uci() << '\n';
    std::cout << "  From: " << square_name(m2.from()) << '\n';
    std::cout << "  To:   " << square_name(m2.to()) << '\n';
    std::cout << "  Flags: " << int(m2.flags()) << " (promotion)\n";

    // Test Move::from_string
    Move m3 = Move::from_uci("g1f3");
    std::cout << "Parsed Move 3: " << m3.uci() << '\n';

    std::cout << std::endl;
    return 0;
}
