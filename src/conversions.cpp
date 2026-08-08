#include "conversions.hpp"

#include <cassert>
#include <iterator>

std::string conversions::bytes_to_hex(const std::string &bytes)
{
    static const char* hex_digits = "0123456789abcdef";
    std::string out;
    out.reserve(bytes.size() * 2);
    for (unsigned char c : bytes) {
        out.push_back(hex_digits[(c >> 4) & 0xF]);
        out.push_back(hex_digits[c & 0xF]);
    }
    return out;
}

std::string conversions::hex_to_bytes(const std::string &hex)
{
    std::string out;
    out.reserve(hex.size() / 2);

    auto hex_value = [](char c) -> unsigned char {
        if (c >= '0' && c <= '9') return static_cast<unsigned char>(c - '0');
        if (c >= 'a' && c <= 'f') return static_cast<unsigned char>(10 + c - 'a');
        if (c >= 'A' && c <= 'F') return static_cast<unsigned char>(10 + c - 'A');
        assert(false);
        return 0;
    };

    assert(hex.size() % 2 == 0);
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        unsigned char high = hex_value(hex[i]);
        unsigned char low = hex_value(hex[i + 1]);
        out.push_back(static_cast<char>((high << 4) | low));
    }

    return out;
}

std::string conversions::bits_to_bytes(const std::string &bits)
{
    std::string out;
    assert(bits.size() % 8 == 0);
    out.reserve(bits.size() / 8);

    for (std::size_t i = 0; i < bits.size(); i += 8) {
        unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            byte += static_cast<unsigned char>((bits[i + j] == '1' ? 1 : 0) << (7 - j));
        out.push_back(static_cast<char>(byte));
    }

    return out;
}

std::string conversions::bytes_to_bits(const std::string &bytes)
{
    std::string out;
    out.reserve(bytes.size() * 8);

    for (unsigned char byte : bytes) {
        for (int i = 7; i >= 0; --i)
            out.push_back(((byte >> i) & 1) ? '1' : '0');
    }

    return out;
}

std::string conversions::hex_to_bits(const std::string &hex)
{
    return bytes_to_bits(hex_to_bytes(hex));
}

std::string conversions::bits_to_hex(const std::string &bits)
{
    return bytes_to_hex(bits_to_bytes(bits));
}

std::string conversions::pad_left(const std::string &bits)
{
    if (bits.size() % 8 != 0)
        return std::string(8 - bits.size() % 8, '0') + bits;
    return bits;
}
