#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "word_serializer.hpp"
#include "encoder.hpp"
#include "crc.hpp"
#include "rinex_manager/rinex_manager.hpp"

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

    py::class_<RinexManager>(m, "RinexManager")
        .def(py::init<>())
        .def("load", &RinexManager::load)
        .def("get_serialized_rinex_records", [](RinexManager &mgr) {
            std::vector<py::bytes> serialized;
            for (const auto &record : mgr.get_rinex_records()) {
                serialized.emplace_back(record.SerializeAsString());
            }
            return serialized;
        });
}