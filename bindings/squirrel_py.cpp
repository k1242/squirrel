#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../engine/Board.hpp"
#include "../engine/Move.hpp"
#include "../engine/movegen.hpp"

namespace py = pybind11;

PYBIND11_MODULE(squirrel, m)
{
    py::class_<Board>(m, "Board")
        // constructors
        .def(py::init<>())
        .def(py::init<const std::string &>(), py::arg("fen"))
        .def(py::init<const Board&, const Move&>(), py::arg("prev"), py::arg("move"))
        // core API
        .def("parse_fen",  &Board::parse_fen, py::arg("fen"))
        .def("fen",        &Board::fen)
        .def("legal_moves",&Board::legal_moves)
        // public members
        .def_readwrite("_bitboards",          &Board::bitboards)
        .def_readwrite("_side_to_move",       &Board::side_to_move)
        .def_readwrite("_castling_rights",    &Board::castling_rights)
        .def_readwrite("_en_passant_square",  &Board::en_passant_square)
        .def_readwrite("_halfmove_clock",     &Board::halfmove_clock)
        .def_readwrite("_fullmove_number",    &Board::fullmove_number);

    py::class_<Move>(m, "Move")
        .def(py::init<int,int,int>(),
             py::arg("_from"), py::arg("_to"), py::arg("_flags") = 0)
        .def_static("from_uci", &Move::from_uci, py::arg("move"))
        .def("uci",             &Move::uci)
        .def_property_readonly("_from",  &Move::from)
        .def_property_readonly("_to",    &Move::to)
        .def_property_readonly("_flags", &Move::flags);

    m.def("perft", &perft, py::arg("board"), py::arg("depth"));
}
