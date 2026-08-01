#include "word_definition.hpp"

#include <fstream>
#include <sstream>
#include <cmath>
#include <regex>
#include <stdexcept>
#include <algorithm>

namespace {
    void strip(std::string &str);
    double get_scale(const std::string& scale_str, int n_bts, bool is_unsigned);
    bool get_signed(const std::string& signed_str);
    double get_offset(const std::string& offset_str);
    std::size_t get_n_bits(const std::string &str);
}

WordDefinition WordDefinition::get_word_config(const std::string& file_path) {
    WordDefinition def;
    def.name = file_path;

    std::ifstream file(file_path);
    if (!file.is_open()) throw std::runtime_error("Could not open file");

    std::string line;
    std::getline(file, line); // Skip header

    int line_nr = 1;

    while (std::getline(file, line)) {
        line_nr++;
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        while (std::getline(ss, item, ',')) {
            strip(item);
            tokens.push_back(item);
        }

        if (tokens.size() < 2) throw std::runtime_error("Invalid CSV format");
        
        // if there was a trailing comma, stringstream won't produce a token for the empty part, manually pad it
        std::size_t comma_count = std::count(line.begin(), line.end(), ',');
        while (tokens.size() < comma_count + 1) {
            tokens.push_back("");
        }

        LineValues lv;
        try {
            lv.name = tokens[0];
            lv.n_bts = get_n_bits(tokens[1]);
            lv.is_unsigned = tokens.size() < 4 ? false : get_signed(tokens[3]);
            lv.scale = tokens.size() < 3 ? 1.0 : get_scale(tokens[2], lv.n_bts, lv.is_unsigned);
            lv.offset = tokens.size() < 5 ? 0.0 : get_offset(tokens[4]);
        } catch (const std::exception& e) {
            throw std::runtime_error("Error parsing line " + std::to_string(line_nr) + ": " + e.what() + 
            "\nLine content: " + line);
        }

        def.entries.push_back(lv);
    }
    return def;
}


namespace {
    void strip(std::string &str)
    {
        if (!str.empty()) {
            str.erase(0, str.find_first_not_of(" \t\r\n"));
            size_t pos = str.find_last_not_of(" \t\r\n");
            if (pos != std::string::npos) {
                str.erase(pos + 1);
            } else {
                str.clear(); // if string only contains whitespaces
            }
        }
    }

    double get_scale(const std::string& scale_str, int n_bts, bool is_unsigned) {
        if (scale_str.empty()) return 1.0;
        if (scale_str == "semi-circle") {
            if (is_unsigned) throw std::runtime_error("A value scaled as 'semi-circle' cannot be unsigned");
            return std::pow(2, -(n_bts - 1)) * M_PI;
        }

        std::regex e("([0-9]+)\\*\\*(-?[0-9]+)");
        std::smatch m;
        if (std::regex_match(scale_str, m, e)) {
            double base = std::stod(m[1]);
            double exp = std::stod(m[2]);
            return std::pow(base, exp);
        }

        return std::stod(scale_str);
    }

    bool get_signed(const std::string& signed_str) {
        if (signed_str == "signed" || signed_str.empty()) return false;
        if (signed_str == "unsigned") return true;
        throw std::invalid_argument("Signed string should be 'signed' or 'unsigned'");
    }

    double get_offset(const std::string& offset_str) {
        if (offset_str.empty()) return 0.0;
        return std::stod(offset_str);
    }

    std::size_t get_n_bits(const std::string &str)
    {
        try {
            return std::stoi(str);
        } catch(std::exception& e) {
            throw std::runtime_error("Invalid or missing bits count for " + str);
        }
    }
}