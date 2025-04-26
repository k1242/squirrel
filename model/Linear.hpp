#include "cnpy.h"
#include "lin.hpp"

struct Linear {
    cnpy::NpyArray hW, hA;
    lin::mat W;
    lin::vec A;
    lin::vec tmp;

    Linear(const std::string& pathW, const std::string& pathA)
        : W(lin::loadm(pathW, hW)), 
          A(lin::loadv(pathA, hA)),
          tmp{ new float[W.shape[0]], { W.shape[0], 1 } }
        {}

    void operator()(lin::vec& x) {
        lin::vmmc(x, W, tmp);
        lin::vavi(tmp, A);
        x.data = tmp.data;
        x.shape[0] = tmp.shape[0];
    }

    ~Linear() {
        delete[] tmp.data;
    }

};


inline void relu(lin::vec& v) {
    for (int i = 0; i < v.shape[0] * v.shape[1]; ++i) 
        v.data[i] = (v.data[i] < 0.0f) ? 0.0f : v.data[i];
}


// #pragma once
// #include "cnpy.h"
// #include "lin.hpp"

// struct Linear {
//     cnpy::NpyArray hW{}, hA{};
//     lin::mat        W{};
//     lin::vec        A{};
//     lin::vec        tmp{};

//     explicit Linear(const std::string& pathW, const std::string& pathA);

//     void operator()(lin::vec& x);

//     ~Linear();
// };

// void relu(lin::vec& v);
