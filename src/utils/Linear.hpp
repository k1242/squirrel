#include "cnpy.h"
#include "lin.hpp"

struct Linear {
    cnpy::NpyArray hW, hA;
    lin::mat W;
    lin::vec A;
    lin::vec inner;

    Linear(const std::string& pathW, const std::string& pathA)
        : W(lin::loadm(pathW, hW)), 
          A(lin::loadv(pathA, hA)),
          inner{ new float[W.shape[1]], { W.shape[1], 1 } }
        {}

    inline void relu() {
        for (int i = 0; i < inner.shape[0]; ++i) 
            inner[i] = (inner[i] < 0.0f) ? 0.0f : inner[i];
    }

    void operator()(lin::vec& x) {
        lin::vmmc(x, W, inner);
        lin::vavi(inner, A);
    }

    // ~Linear() {delete[] inner.data;}

};


