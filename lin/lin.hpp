#pragma once
#include <cstddef>
#include "cnpy.h"

namespace lin {

struct buf {
    float* data;
    int shape[2];

    float& operator[](int j) { return data[j]; }
    const float& operator[](int j) const { return data[j]; }
};

using vec = buf;
using mat = buf;

const float* row(const mat& M, int k);

// vector @ matrix
void vmmc(const vec& v, const mat& M, vec& out);    // out = M @ v    (copy)

// vector ± vector
void vavi(vec& v, const vec& u);                    // v += u         (in-place)
void vsvi(vec& v, const vec& u);                    // v -= u         (in-place)

// vector ± row(matrix)
void vami(vec& v, const mat& M, int k);             // v += row_k(M)  (in-place)
void vsmi(vec& v, const mat& M, int k);             // v -= row_k(M)  (in-place)

inline mat loadm(const std::string& path, cnpy::NpyArray& holder) {
    holder = cnpy::npy_load(path);
    auto* ptr = holder.data<float>();
    int rows  = holder.shape[0];
    int cols  = holder.shape.size() > 1 ? holder.shape[1] : 1;
    return mat{ ptr, { rows, cols } };
}

inline vec loadv(const std::string& path, cnpy::NpyArray& holder) {
    holder = cnpy::npy_load(path);
    auto* ptr = holder.data<float>();
    int len   = holder.shape[0];
    return vec{ ptr, { len, 1 } };
}

}
