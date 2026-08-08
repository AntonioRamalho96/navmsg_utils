#pragma once
#include <string>


namespace conversions
{
    // Convert a byte string to a hexadecimal string (two hex chars per byte).
    std::string bytes_to_hex(const std::string &bytes);

    // Convert a hexadecimal string to a byte string (two hex chars per byte).
    std::string hex_to_bytes(const std::string &hex);

    // Convert a string of '1' and '0' characters to a byte string.
    std::string bits_to_bytes(const std::string &bits);

    // Convert a byte string to a string of '1' and '0' characters.
    std::string bytes_to_bits(const std::string &bytes);

    // Convert a hexadecimal string to a string of '1' and '0' characters.
    std::string hex_to_bits(const std::string &hex);

    // Convert a string of '1' and '0' characters to a hexadecimal string.
    std::string bits_to_hex(const std::string &bits);
    
    // Padd on the left with zeros to make length divisible by 8.
    std::string pad_left(const std::string &bits);
}