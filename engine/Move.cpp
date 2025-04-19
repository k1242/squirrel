#include "Move.hpp"
#include "notation.hpp"

Move::Move(int from, int to, int flags) {
    data = (flags << 12) | (to << 6) | from;
}

int Move::from() const { return data & 0x3F; }
int Move::to() const { return (data >> 6) & 0x3F; }
int Move::flags() const { return (data >> 12) & 0xF; }

Move Move::from_uci(const std::string& move_str) {
    int from = square_index(move_str.substr(0, 2));
    int to = square_index(move_str.substr(2, 2));
    int flag = 0;
    if (move_str.size() == 5) {
        flag = promotion_flag(move_str[4]);
    }
    return Move(from, to, flag);
}

std::string Move::uci() const {
    std::string res = square_name(from()) + square_name(to());
    char suffix = promotion_suffix(flags());
    if (suffix != '\0') res += suffix;
    return res;
}
