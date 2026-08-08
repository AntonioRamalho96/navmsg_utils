#pragma once

#include <string>


class Crc
{
public:
    static std::string crc24(const std::string& input);
};