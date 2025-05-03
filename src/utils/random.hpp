#pragma once
#include <cstdint>

class PCG64 {
    uint64_t state = 0x853c49e6748fea9bULL;

    static constexpr uint64_t multiplier = 6364136223846793005ULL;
    static constexpr uint64_t increment  = 1442695040888963407ULL;

public:
    uint64_t operator()() {
        uint64_t x = state;
        state = x * multiplier + increment;

        uint64_t xorshifted = ((x >> 18) ^ x) >> 27;
        uint64_t rot = x >> 59;
        return (xorshifted >> rot) | (xorshifted << ((-rot) & 63));
    }
};


class PCG64DXSM {
    __uint128_t state = (__uint128_t(0x4d595df4d0f33173ULL) << 64) | 0x853c49e6748fea9bULL;
    static constexpr __uint128_t multiplier = (__uint128_t(0x5851f42d4c957f2dULL) << 64) | 0x14057b7ef767814fULL;

    static uint64_t dxsm(uint64_t hi, uint64_t lo) {
        uint64_t x = hi ^ (hi >> 32);
        x *= 0xda942042e4dd58b5ULL;
        x ^= x >> 32;
        x += lo;
        return x;
    }

public:
    uint64_t operator()() {
        state *= multiplier;
        uint64_t hi = uint64_t(state >> 64);
        uint64_t lo = uint64_t(state);
        return dxsm(hi, lo);
    }
};


class Xoshiro256pp {
private:
    uint64_t s[4] = {
        0xA3E5F17C1A9279B1ULL,
        0x59C3B37D0F42D8EDULL,
        0xE9934F6AB7B23C6FULL,
        0x7135B1F231D8C0AAULL
    };

    static uint64_t rotl(uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }


public:
    uint64_t operator()() {
        uint64_t r = rotl(s[0] + s[3], 23) + s[0];
        uint64_t t = s[1] << 17;

        s[2] ^= s[0]; s[3] ^= s[1];
        s[1] ^= s[2]; s[0] ^= s[3];
        s[2] ^= t;
        s[3] = rotl(s[3], 45);

        return r;
    }
};


class XorShift64 {
    uint64_t state = 88172645463325252ULL;

public:
    uint64_t operator()() {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }
};


class RndMagic {
    XorShift64 rng;

public:
    uint64_t operator()() {
        return rng() & rng() & rng();
    }
};
