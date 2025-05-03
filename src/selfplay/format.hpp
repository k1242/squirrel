#pragma once
#include "types.hpp"

// on‑disk header
struct GameHeader {
    I8  result;      // +1 white, 0 draw, ‑1 black
    U16 halfmoves;   // number of plies
    U8  version;     // =1
};

// one policy entry
struct PolicyItemBin {
    U16 move;
    U32 visits;
};