#include "crc.hpp"

#include <cstdint>

#include "conversions.hpp"

namespace 
{
    std::string crc_24_impl(const std::string& data);
}

std::string Crc::crc24(const std::string &bits) 
{ 
    using namespace conversions;
    std::string input_bytes = bits_to_bytes(pad_left(bits));
    return bytes_to_bits(crc_24_impl(input_bytes));
}

namespace
{
    // Polynomial without the x^24 term.
    static constexpr uint32_t POLY = 0x864CFB;

    // Computes a 24-bit CRC.
    uint32_t compute(const std::string& data)
    {
        uint32_t crc = 0;

        for (unsigned char byte : data)
        {
            for (int i = 0; i < 8; ++i)
            {
                bool bit = (byte >> (7 - i)) & 1;
                bool top = (crc >> 23) & 1;

                crc = (crc << 1) & 0xFFFFFF;

                if (top ^ bit)
                    crc ^= POLY;
            }
        }

        return crc;
    }

    std::string crc_24_impl(const std::string& data)
    {
        uint32_t crc = compute(data);
        std::string out(3, '\0');
        out[0] = static_cast<char>((crc >> 16) & 0xFF);
        out[1] = static_cast<char>((crc >> 8) & 0xFF);
        out[2] = static_cast<char>(crc & 0xFF);
        return out;
    }
}


