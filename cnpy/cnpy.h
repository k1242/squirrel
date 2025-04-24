// Copyright (C) 2011  Carl Rogers
// Released under MIT License
#pragma once

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <typeinfo>
#include <vector>

namespace cnpy {

struct NpyArray {
    NpyArray(const std::vector<size_t>& _shape,
             size_t _word_size,
             bool _fortran_order)
        : shape(_shape),
          word_size(_word_size),
          fortran_order(_fortran_order) {
        num_vals = 1;
        for (size_t d : shape) num_vals *= d;
        data_holder =
            std::make_shared<std::vector<char>>(num_vals * word_size);
    }

    NpyArray() : word_size(0), fortran_order(false), num_vals(0) {}

    template <typename T>       T* data()       { return reinterpret_cast<T*>(&(*data_holder)[0]); }
    template <typename T> const T* data() const { return reinterpret_cast<const T*>(&(*data_holder)[0]); }

    template <typename T>
    std::vector<T> as_vec() const { return {data<T>(), data<T>() + num_vals}; }

    size_t num_bytes() const { return data_holder->size(); }

    std::shared_ptr<std::vector<char>> data_holder;
    std::vector<size_t> shape;
    size_t   word_size{};
    bool     fortran_order{};
    size_t   num_vals{};
};

// ── helpers ───────────────────────────────────────────────────────────
char BigEndianTest();
char map_type(const std::type_info& t);

template <typename T>
std::vector<char> create_npy_header(const std::vector<size_t>& shape);

void parse_npy_header(FILE*         fp,
                      size_t&       word_size,
                      std::vector<size_t>& shape,
                      bool&         fortran_order);

void parse_npy_header(unsigned char* buffer,
                      size_t&        word_size,
                      std::vector<size_t>& shape,
                      bool&          fortran_order);

NpyArray npy_load(const std::string& fname);

// convenient wrappers
template <typename T>
void npy_save(const std::string&           fname,
              const T*                     data,
              const std::vector<size_t>&   shape,
              std::string                  mode = "w");

template <typename T>
void npy_save(const std::string& fname,
              const std::vector<T>& data,
              std::string           mode = "w");

// operator+= helpers for building byte-vectors
template <typename T>
std::vector<char>& operator+=(std::vector<char>& lhs, const T rhs) {
    for (size_t byte = 0; byte < sizeof(T); ++byte)
        lhs.push_back(*((char*)&rhs + byte));     // little-endian
    return lhs;
}
template <> std::vector<char>& operator+=(std::vector<char>& lhs, const std::string rhs);
template <> std::vector<char>& operator+=(std::vector<char>& lhs, const char* rhs);

// ──────────────────────────────────────────────────────────────────────
} // namespace cnpy

// -------------  template impls included in header -------------
#include "cnpy_impl.inl"
