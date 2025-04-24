#pragma once
#include <numeric>
#include <sstream>

namespace cnpy {

template <typename T>
void npy_save(const std::string&         fname,
              const T*                   data,
              const std::vector<size_t>& shape,
              std::string                mode) {
    FILE* fp = nullptr;
    std::vector<size_t> true_shape;

    if (mode == "a") fp = std::fopen(fname.c_str(), "r+b");

    if (fp) {
        size_t word_size; bool fortran;
        parse_npy_header(fp, word_size, true_shape, fortran);
        assert(!fortran && "Fortran order not supported");
        assert(word_size == sizeof(T));

        assert(true_shape.size() == shape.size());
        for (size_t i = 1; i < shape.size(); ++i)
            assert(shape[i] == true_shape[i]);

        true_shape[0] += shape[0];            // grow first dimension
    } else {
        fp = std::fopen(fname.c_str(), "wb");
        true_shape = shape;
    }

    auto header = create_npy_header<T>(true_shape);
    const size_t nels =
        std::accumulate(shape.begin(), shape.end(), size_t{1}, std::multiplies<size_t>());

    std::fseek(fp, 0, SEEK_SET);
    std::fwrite(header.data(), 1, header.size(), fp);
    std::fseek(fp, 0, SEEK_END);
    std::fwrite(data, sizeof(T), nels, fp);
    std::fclose(fp);
}

template <typename T>
void npy_save(const std::string& fname,
              const std::vector<T>& data,
              std::string           mode) {
    npy_save(fname, data.data(), {data.size()}, std::move(mode));
}

template <typename T>
std::vector<char> create_npy_header(const std::vector<size_t>& shape) {
    std::vector<char> dict;
    dict += "{'descr': '";
    dict += BigEndianTest();
    dict += map_type(typeid(T));
    dict += std::to_string(sizeof(T));
    dict += "', 'fortran_order': False, 'shape': (";
    dict += std::to_string(shape[0]);
    for (size_t i = 1; i < shape.size(); ++i) {
        dict += ", ";
        dict += std::to_string(shape[i]);
    }
    if (shape.size() == 1) dict += ",";
    dict += "), }";

    int pad = 16 - (10 + int(dict.size())) % 16;  // preamble: 10 байт
    dict.insert(dict.end(), pad, ' ');
    dict.back() = '\n';

    std::vector<char> header;
    header += char(0x93);              // magic
    header += "NUMPY";
    header += char(0x01);              // major
    header += char(0x00);              // minor
    header += uint16_t(dict.size());   // header length (little-endian)
    header.insert(header.end(), dict.begin(), dict.end());
    return header;
}

} // namespace cnpy
