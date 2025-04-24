#include "lin.hpp"
#include <cassert>

namespace lin {

//  helpers
const float* row(const mat& M, int k) {
    return M.data + k * M.shape[1];
}

//  vector @ matrix (copy)
void vmmc(const vec& v, const mat& M, vec& out) {
    assert(M.shape[1] == v.shape[0] && v.shape[1] == 1);
    assert(out.shape[0] >= M.shape[0] && out.shape[1] == 1);

    const int rows = M.shape[0];
    const int cols = M.shape[1];

    const float* vptr   = v.data;
    float*       outptr = out.data;
    const float* mptr   = M.data;

    for (int r = 0; r < rows; ++r, mptr += cols) {
        float acc = 0.0f;
        for (int c = 0; c < cols; ++c)
            acc += mptr[c] * vptr[c];
        outptr[r] = acc;
    }

    out.shape[0] = rows;
}


// vector ± vector
void vavi(vec& v, const vec& u) {
    assert(v.shape[0] == u.shape[0] && v.shape[1] == 1 && u.shape[1] == 1);
    const int n = v.shape[0];
    for (int i = 0; i < n; ++i) v.data[i] += u.data[i];
}

void vsvi(vec& v, const vec& u) {
    assert(v.shape[0] == u.shape[0] && v.shape[1] == 1 && u.shape[1] == 1);
    const int n = v.shape[0];
    for (int i = 0; i < n; ++i) v.data[i] -= u.data[i];
}


// vector ± row(matrix) in-place
void vami(vec& v, const mat& M, int k) {
    assert(k >= 0 && k < M.shape[0]);
    assert(M.shape[1] == v.shape[0] && v.shape[1] == 1);
    const int n = v.shape[0];
    const float* rowPtr = row(M, k);
    for (int i = 0; i < n; ++i) v.data[i] += rowPtr[i];
}

void vsmi(vec& v, const mat& M, int k) {
    assert(k >= 0 && k < M.shape[0]);
    assert(M.shape[1] == v.shape[0] && v.shape[1] == 1);
    const int n = v.shape[0];
    const float* rowPtr = row(M, k);
    for (int i = 0; i < n; ++i) v.data[i] -= rowPtr[i];
}

}
