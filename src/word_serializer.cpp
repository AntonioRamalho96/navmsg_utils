#include "word_serializer.hpp"

#include <cstdint>
#include <string>
#include <iostream>
#include <cmath>
#include <stdexcept>

#include "rinex_record.pb.h"

namespace {
    
    std::string to_bits(double value, int bits, bool is_unsigned);
    double get_field_value(
        const rinex_manager::RinexNavRecord& record, 
        const std::string& field_name, 
        uint32_t wn, uint32_t tow
    );
}

word_serializer::word_serializer(const std::string& file_path) : word_definition(WordDefinition::get_word_config(file_path)) {}

std::string word_serializer::serialize(const std::string& serialized_record, uint32_t wn, uint32_t tow) const {
    rinex_manager::RinexNavRecord record;
    if (!record.ParseFromString(serialized_record)) {
        throw std::runtime_error("Failed to parse RinexNavRecord");
    }

    std::string result;
    for (const auto& entry : word_definition.entries) {
        double value = get_field_value(record, entry.name, wn, tow);
        value = (value - entry.offset) / entry.scale;
        result += to_bits(value, entry.n_bts, entry.is_unsigned);
    }
    return result;
}


namespace {

std::string to_bits(double value, int bits, bool is_unsigned) {
    double min_val = is_unsigned ? 0 : -std::pow(2, bits - 1);
    double max_val = is_unsigned ? std::pow(2, bits) - 1 : std::pow(2, bits - 1);

    if (value >= min_val && value < max_val) {
        long long int_val = std::floor(value);
        if (int_val < 0) int_val += std::pow(2, bits);
        std::string bit_str;
        for (int i = bits - 1; i >= 0; --i) {
            bit_str += ((int_val >> i) & 1) ? "1" : "0";
        }
        return bit_str;
    } else {
        std::cout << value << " out of bunds for " << bits << " bits (" << (is_unsigned ? "unsigned" : "signed") << ")\n";
        return std::string(bits, '1');
    }
}

double get_field_value(const rinex_manager::RinexNavRecord& record, 
                       const std::string& field_name, 
                       uint32_t wn, uint32_t tow) 
{
    if( field_name == "EMPTY") return 0;
    if( field_name.substr(0,4) == "VAL:") return std::stod(field_name.substr(4));
    if( field_name == "WN") return wn;
    if( field_name == "TOW") return tow;

    const google::protobuf::Reflection* reflection = record.GetReflection();
    const google::protobuf::Descriptor* descriptor = record.GetDescriptor();
    const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName(field_name);

    if (!field) {
        throw std::runtime_error("Invalid name " + field_name + " in word definition");
    }

    switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            return reflection->GetInt32(record, field);
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            return reflection->GetDouble(record, field);
        default:
            throw std::runtime_error("Unsupported field type for " + field_name);
    }
}
}