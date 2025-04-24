// Copyright (C) 2011  Carl Rogers
// Released under MIT License
#include "cnpy.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <regex>
#include <stdexcept>

using namespace cnpy;

// ── helpers ───────────────────────────────────────────────────────────
char cnpy::BigEndianTest() {
    int x = 1;
    return (reinterpret_cast<char*>(&x)[0]) ? '<' : '>';
}

char cnpy::map_type(const std::type_info& t) {
    if (t == typeid(float)  || t == typeid(double) || t == typeid(long double))        return 'f';
    if (t == typeid(int)    || t == typeid(char)   || t == typeid(short) ||
        t == typeid(long)   || t == typeid(long long))                                 return 'i';
    if (t == typeid(unsigned char)  || t == typeid(unsigned short) ||
        t == typeid(unsigned long) || t == typeid(unsigned long long) ||
        t == typeid(unsigned int))                                                     return 'u';
    if (t == typeid(bool))                                                             return 'b';
    return '?';
}

template <>
std::vector<char>& cnpy::operator+=(std::vector<char>& lhs, const std::string rhs) {
    lhs.insert(lhs.end(), rhs.begin(), rhs.end());
    return lhs;
}
template <>
std::vector<char>& cnpy::operator+=(std::vector<char>& lhs, const char* rhs) {
    lhs.insert(lhs.end(), rhs, rhs + std::strlen(rhs));
    return lhs;
}

// ── header parsing ───────────────────────────────────────────────────
void cnpy::parse_npy_header(unsigned char* buffer,
                            size_t&        word_size,
                            std::vector<size_t>& shape,
                            bool&          fortran_order) {
    uint16_t header_len = *reinterpret_cast<uint16_t*>(buffer + 8);
    std::string header(reinterpret_cast<char*>(buffer + 9), header_len);

    // fortran_order
    auto loc = header.find("fortran_order") + 16;
    fortran_order = header.substr(loc, 4) == "True";

    // shape
    loc = header.find('(');
    auto loc2 = header.find(')');
    std::regex num_rgx("[0-9]+");
    std::smatch sm;
    shape.clear();
    std::string shp = header.substr(loc + 1, loc2 - loc - 1);
    while (std::regex_search(shp, sm, num_rgx)) {
        shape.push_back(std::stoul(sm[0].str()));
        shp = sm.suffix().str();
    }

    // word size
    loc = header.find("descr") + 9;
    bool little = header[loc] == '<' || header[loc] == '|';
    assert(little && "Big-endian data not supported");
    word_size = std::stoul(header.substr(loc + 2, header.find('\'', loc + 2) - (loc + 2)));
}

void cnpy::parse_npy_header(FILE* fp,
                            size_t&        word_size,
                            std::vector<size_t>& shape,
                            bool&          fortran_order) {
    char buffer[256];
    if (fread(buffer, 1, 11, fp) != 11)
        throw std::runtime_error("parse_npy_header: failed fread");
    std::string header = fgets(buffer, 256, fp);
    if (header.back() != '\n')
        throw std::runtime_error("parse_npy_header: malformed header");

    // fortran_order
    auto loc = header.find("fortran_order");
    if (loc == std::string::npos)
        throw std::runtime_error("parse_npy_header: 'fortran_order' not found");
    fortran_order = header.substr(loc + 16, 4) == "True";

    // shape
    auto loc1 = header.find('(');
    auto loc2 = header.find(')');
    if (loc1 == std::string::npos || loc2 == std::string::npos)
        throw std::runtime_error("parse_npy_header: shape not found");
    std::regex num_rgx("[0-9]+");
    std::smatch sm;
    shape.clear();
    std::string shp = header.substr(loc1 + 1, loc2 - loc1 - 1);
    while (std::regex_search(shp, sm, num_rgx)) {
        shape.push_back(std::stoul(sm[0].str()));
        shp = sm.suffix().str();
    }

    // word size
    loc = header.find("descr");
    if (loc == std::string::npos)
        throw std::runtime_error("parse_npy_header: 'descr' not found");
    loc += 9;
    bool little = header[loc] == '<' || header[loc] == '|';
    assert(little && "Big-endian data not supported");
    word_size = std::stoul(header.substr(loc + 2, header.find('\'', loc + 2) - (loc + 2)));
}

// ── npy load helper ──────────────────────────────────────────────────
static NpyArray load_npy_file(FILE* fp) {
    std::vector<size_t> shape;
    size_t   word_size;
    bool     fortran;
    cnpy::parse_npy_header(fp, word_size, shape, fortran);

    NpyArray arr(shape, word_size, fortran);
    if (fread(arr.data<char>(), 1, arr.num_bytes(), fp) != arr.num_bytes())
        throw std::runtime_error("npy_load: failed fread (data)");
    return arr;
}

NpyArray cnpy::npy_load(const std::string& fname) {
    FILE* fp = std::fopen(fname.c_str(), "rb");
    if (!fp) throw std::runtime_error("npy_load: unable to open '" + fname + '\'');

    NpyArray arr = load_npy_file(fp);
    std::fclose(fp);
    return arr;
}
