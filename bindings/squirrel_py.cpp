// bindings/squirrel_py.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../engine/Board.hpp"
#include "../engine/Move.hpp"

namespace py = pybind11;

PYBIND11_MODULE(squirrel, m)
{
    py::class_<Board>(m, "Board")
        .def(py::init<>())
        .def(py::init<const std::string &>())
        .def(py::init<const Board&, const Move&>())
        .def("parse_fen", &Board::parse_fen)
        .def("fen", &Board::fen)
        .def_readwrite("bitboards", &Board::bitboards)
        .def_readwrite("side_to_move", &Board::side_to_move);

    py::class_<Move>(m, "Move")
        .def(py::init<int,int,int>())
        .def_static("from_uci", &Move::from_uci)
        .def("uci", &Move::uci)
        .def_property_readonly("_from", &Move::from)   // «from» — ключевое слово в Python
        .def_property_readonly("_to",    &Move::to)
        .def_property_readonly("_flags", &Move::flags);
}
