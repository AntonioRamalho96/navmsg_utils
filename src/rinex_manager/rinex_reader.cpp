#include "rinex_reader.hpp"
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>

RinexReader::RinexReader(const std::string &rinex_path)
{
    file.open(rinex_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open rinex file at " + rinex_path);
    }
    read_pos.file_path = rinex_path;
    next_line();
}

bool RinexReader::next_line()
{
    read_pos.offset = 0;
    read_pos.line_nr++;
    return static_cast<bool>(std::getline(file, current_line));
}

std::string RinexReader::get_char(size_t size)
{
    std::string result = current_line.substr(read_pos.offset, size);
    read_pos.offset+=size;
    return result;
}

void RinexReader::skip_space(size_t n)
{
    std::string expected_spaces(n, ' ');
    if (get_char(n) != expected_spaces) {
        std::ostringstream message;
        message << "Expecting " << n << " space(s) in: " << read_pos.to_str(n);
        throw std::invalid_argument(message.str());
    }
}

int RinexReader::get_int(size_t n)
{
    std::string chars = get_char(n);
    for (char c : chars)
        if (!std::isdigit(c)) {
            std::ostringstream message;
            message << "Expecting " << n << " integers in: " << read_pos.to_str(n);
            throw std::invalid_argument(message.str());
        }
    return std::atoi(chars.c_str());
}

double RinexReader::get_19_char_double()
{
    std::string chars = get_char(19);
    if (
        !(chars[0] == ' ' || chars[0] == '-') ||   // Signal or space
        !(std::isdigit(chars[1])) ||                // digit
        !(chars[2] == '.') ||                      // decimal separator
        !(std::isdigit(chars[3]) && std::isdigit(chars[4]) && std::isdigit(chars[5]) && std::isdigit(chars[6]) && std::isdigit(chars[7]) && std::isdigit(chars[8]) && std::isdigit(chars[9]) && std::isdigit(chars[10]) && std::isdigit(chars[11]) && std::isdigit(chars[12]) && std::isdigit(chars[13]) && std::isdigit(chars[14])) || // Digits only
        !(chars[15] == 'e') ||                     // exponent
        !(chars[16] == '+' || chars[16] == '-') || // exponent sign
        !(std::isdigit(chars[17]) && std::isdigit(chars[18])) // exponent value
        ) {
        std::ostringstream message;
        message << "Expecting a scientific number in: " << read_pos.to_str(19) << " but got: " << chars;
        throw std::invalid_argument(message.str());
    }
    return std::atof(chars.c_str());
}

std::array<bool, 10> RinexReader::get_19_char_bits()
{
    int bits_as_int = get_19_char_int();
    std::array<bool, 10> bits{};
    for (size_t i = 0; i < bits.size(); ++i)
    {
        bits[i] = (bits_as_int >>  i) & 1;
    }
    return bits;
}

int RinexReader::get_19_char_int()
{
    return std::lround(get_19_char_double());
}

bool RinexReader::next_record()
{
    bool still_reading;
    do{
        still_reading = next_line();
    }
    while ( still_reading  && current_line[0] != '>');
    return still_reading;
}

RinexReader::ReadPosition RinexReader::get_pos() const
{
    return read_pos;
}

std::string RinexReader::ReadPosition::to_str(int offset_sub) const
{
    return  file_path + ":" + std::to_string(line_nr) + " char " + std::to_string(offset - offset_sub);
}