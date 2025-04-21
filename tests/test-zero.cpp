#include <iostream>
#include <array>
#include <chrono>
#include <cstdint>

using U64 = uint64_t;
constexpr int N = 12;
constexpr int ITERATIONS = 10'000'000;

void test_std_array() {
    std::array<U64, N> a{};
    std::array<U64, N> b{};

    for (int i = 0; i < N; ++i)
        a[i] = i;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; ++i)
        b = a;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "std::array copy time: " << diff.count() << " s\n";
}

void test_c_array() {
    U64 a[N]{};
    U64 b[N]{};

    for (int i = 0; i < N; ++i)
        a[i] = i;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; ++i)
        for (int j = 0; j < N; ++j)
            b[j] = a[j];

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "C-array copy time: " << diff.count() << " s\n";
}

int main() {
    test_std_array();
    test_c_array();
    return 0;
}
