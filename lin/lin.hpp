#pragma once
#include <cstddef>
#include <format>
#include <cmath>
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
void vavc(vec& v, const vec& u, vec& out);
void vsvc(vec& v, const vec& u, vec& out);

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

inline void print_vec(const lin::vec& v, const int lim) {
    int n = (lim == -1) ? v.shape[0] : lim;
    std::cout << "[";
    for (int i = 0; i < n; ++i) {
        std::cout << std::format("{:.2f}", v[i]);
        if (i != n - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

inline float L1(const lin::vec& v) {
    float norm = 0.0;
    for (int i = 0; i < v.shape[0]; ++i) 
        norm += std::abs(v[i]);
    return norm / v.shape[0];
}

inline float L2(const lin::vec& v) {
    float norm = 0.0;
    for (int i = 0; i < v.shape[0]; ++i) 
        norm += v[i] * v[i];
    return std::sqrt(norm / v.shape[0]);
}

}
