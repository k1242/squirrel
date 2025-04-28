#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>

struct AccumulatingTimer {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    struct Stats {
        double total_ms = 0.0;
        int count = 0;
    };

    static inline std::unordered_map<std::string, Stats> stats;

    std::string name;
    TimePoint start;

    AccumulatingTimer(const std::string& func_name) : name(func_name), start(Clock::now()) {}

    ~AccumulatingTimer() {
        auto end = Clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - start).count();
        auto& s = stats[name];
        s.total_ms += ms;
        s.count += 1;
    }

    static void print_report() {
        std::cout << "\n--- Profiling Report ---\n";
        std::cout << std::left << std::setw(25) << "Function"
                  << std::right << std::setw(12) << "Total (ms)"
                  << std::setw(12) << "Calls"
                  << std::setw(16) << "Avg / Call (ns)"
                  << '\n';
        std::cout << std::string(65, '-') << '\n';

        for (const auto& [name, s] : stats) {
            std::cout << std::left << std::setw(25) << name
                      << std::right << std::setw(12) << std::fixed << std::setprecision(2) << s.total_ms
                      << std::setw(12) << s.count
                      << std::setw(16) << (s.count ? s.total_ms * 1000000.0 / s.count : 0.0)
                      << '\n';
        }

        std::cout << std::string(65, '-') << '\n';
    }
};

#define PROFILE AccumulatingTimer timer(__func__)
