#pragma once
#include <cstdint>
#include <string>

#include "word_definition.hpp"

namespace rinex_manager { class RinexNavRecord; }

class word_serializer {
public:
    word_serializer(const std::string& file_path);
    std::string serialize(const std::string& serialized_record, uint32_t wn, uint32_t tow) const;

private:
    WordDefinition word_definition;
};
