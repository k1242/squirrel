// src/exec/dump_batch.cpp
#include <filesystem>
#include <random>
#include <vector>
#include <iostream>
#include <bit>
#include <algorithm>
#include <numeric>      // accumulate (stats)
#include <cmath>        // sqrt (stats)
#include <iomanip>
#include "types.hpp"
#include "load.hpp"
#include "cnpy.h"

static void collect_piece(U64 bb, int piece,
                          std::vector<I8>& sqs, std::vector<I8>& pcs)
{
    while (bb) {
        int sq = std::countr_zero(bb);
        sqs.push_back(static_cast<I8>(sq));
        pcs.push_back(static_cast<I8>(piece));
        bb &= bb - 1;
    }
}

static void pad_missing(std::vector<I8>& v) { v.resize(32, -1); }


int main(int argc, char* argv[])
{
    size_t N = (argc >= 2) ? std::stoul(argv[1]) : 4096;
    size_t stats_flag = (argc >= 3) ? std::stoul(argv[2]) : 1;   // 0‑no stats, 1‑stats
    size_t est_pos = N * 200;

    // 1. collect *.bin paths
    std::vector<std::filesystem::path> files;
    for (const auto& e : std::filesystem::directory_iterator("data/games"))
        if (e.path().extension() == ".bin") files.push_back(e.path());

    if (files.size() > N) {
        std::mt19937 rng{std::random_device{}()};
        std::shuffle(files.begin(), files.end(), rng);
        files.resize(N);
    } 

    // storage for whole batch
    std::vector<I8> squares_batch;  // shape (K, 32)
    std::vector<I8> pieces_batch;   // shape (K, 32)
    std::vector<I8> results_batch;  // shape (K,)
    std::vector<I8> side_batch;     // shape (K,)

    squares_batch.reserve(32 * est_pos);
    pieces_batch .reserve(32 * est_pos);
    results_batch.reserve(est_pos);
    side_batch   .reserve(est_pos);

    // stats buffers
    std::vector<size_t> game_len;
    size_t winW = 0, winB = 0, draw = 0;
    double sum_r = 0, sum_v = 0, sum_r2 = 0, sum_v2 = 0, sum_rv = 0;
    size_t n_corr = 0;

    // 2. read games
    std::vector<Board>             boards;
    std::vector<I8>                results;
    std::vector<float>             values;
    std::vector<std::vector<Move>> moves;
    std::vector<std::vector<uint32_t>> visits;

    for (const auto& path : files) {
        if (!load_bin_game(path.string(), boards, results, values, moves, visits)){
            std::cout << "error with " << path.string() << std::endl;
            return 1;
        }

        if (stats_flag) {
            game_len.push_back(boards.size());
            if (!results.empty()) {
                int gres = -results[0]; // +1 = white win
                if (gres ==  1) ++winW;
                else if (gres == -1) ++winB;
                else ++draw;
            }
        }

        for (int idx = 0; idx < boards.size(); ++idx) {
            const Board& pos = boards[idx];

            std::vector<I8> sqs, pcs;
            sqs.reserve(32); pcs.reserve(32);

            for (int p = 0; p < 12; ++p)
                collect_piece(pos.bitboards[p], p, sqs, pcs);

            pad_missing(sqs);
            pad_missing(pcs);

            squares_batch.insert(squares_batch.end(), sqs.begin(), sqs.end());
            pieces_batch .insert(pieces_batch .end(), pcs.begin(), pcs.end());
            results_batch.push_back(results[idx]);
            side_batch   .push_back(pos.side_to_move);

            if (stats_flag) {
                double r = results[idx];
                double v = values[idx];
                sum_r  += r;   sum_v  += v;
                sum_r2 += r*r; sum_v2 += v*v;
                sum_rv += r*v; ++n_corr;
            }
        }
    }

    // 3. save
    std::filesystem::create_directories("data/batch");

    const size_t K = results_batch.size();
    cnpy::npy_save<int8_t>("data/batch/square.npy",
                   squares_batch.data(), {K, 32});
    cnpy::npy_save<int8_t>("data/batch/piece.npy",
                   pieces_batch.data(),  {K, 32});
    cnpy::npy_save<int8_t>("data/batch/result.npy",
                   results_batch.data(), {K});
    cnpy::npy_save<int8_t>("data/batch/side.npy",
                   side_batch.data(),    {K});

    std::cout << "# positions    " << K << "\n";

    // 4. statistics (optional)
    if (stats_flag) {
        size_t G = game_len.size();
        double mean = std::accumulate(game_len.begin(), game_len.end(), 0.0) / G;
        double sqsum = 0.0;
        for (size_t l : game_len) sqsum += (l - mean) * (l - mean);
        double sigma = std::sqrt(sqsum / G);

        double pW = static_cast<double>(winW) / G;
        double pB = static_cast<double>(winB) / G;
        double pD = static_cast<double>(draw) / G;

        double num   = n_corr * sum_rv - sum_r * sum_v;
        double den_r = std::sqrt(n_corr * sum_r2 - sum_r * sum_r);
        double den_v = std::sqrt(n_corr * sum_v2 - sum_v * sum_v);
        double corr  = (den_r && den_v) ? num / (den_r * den_v) : 0.0;

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "avg len        " << mean << " +- " << sigma << '\n';
        std::cout << "P(-1 0 +1)     " << pW << " " <<  pD << " " << pB << '\n';
        std::cout << "corr(res, val) " << corr << '\n';
    }
    return 0;
}
