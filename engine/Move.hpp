#pragma once
#include <cstdint>
#include <string>

class Move {
public:
    Move(int from, int to, int flags = 0);
    static Move from_string(const std::string& move_str);

    int from() const;
    int to() const;
    int flags() const;

    std::string to_string() const;

private:
    uint16_t data;
};
