#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

constexpr int M = 512;  // rows
constexpr int N = 512;  // cols
constexpr int R = 100;    // repeats

alignas(64) float W[M][N];   // weights as float
alignas(64) float x[N];      // input as float
float y_scalar[M];


void matvec(float* out) {
for (int i = 0; i < M; ++i) {
    float s = 0.0f;
    for (int j = 0; j < N; ++j)
        s += W[i][j] * x[j];
    out[i] = s;
}
}

int main() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> dw(-1.0f, 1.0f);
    std::uniform_real_distribution<float> dx(-1.0f, 1.0f);

    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            W[i][j] = dw(rng);

    for (int j = 0; j < N; ++j)
        x[j] = dx(rng);

    std::vector<double> t;
    for (int r = 0; r < R; ++r) {
        auto s = std::chrono::high_resolution_clock::now();
        matvec(y_scalar);
        auto e = std::chrono::high_resolution_clock::now();
        t.push_back(std::chrono::duration<double, std::micro>(e - s).count());
    }
    double t_scalar = std::accumulate(t.begin(), t.end(), 0.0) / t.size();

    std::cout << "Scalar avg: " << t_scalar << " μs\n";
}
