#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "word_serializer.hpp"
#include "encoder.hpp"
#include "crc.hpp"

namespace py = pybind11;

PYBIND11_MODULE(nav_utils, m) {
    py::class_<word_serializer>(m, "WordSerializer")
        .def(py::init<const std::string&>())
        .def("serialize", &word_serializer::serialize)
        .def_static("get_word_config", &WordDefinition::get_word_config);
    py::class_<Encoder>(m, "GalileoEncoder")
        .def_static("encode", &Encoder::encode)
        .def_static("interleave_fnav", &Encoder::interleave_fnav)
        .def_static("deinterleave_fnav", &Encoder::deinterleave_fnav)
        .def_static("interleave_inav", &Encoder::interleave_inav)
        .def_static("deinterleave_inav", &Encoder::deinterleave_inav);
    py::class_<Crc>(m, "Crc")
        .def_static("crc24", &Crc::crc24);
}