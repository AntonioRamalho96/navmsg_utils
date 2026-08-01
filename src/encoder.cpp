#include "encoder.hpp"
#include <algorithm>
#include <cassert>
#include <functional>
#include <numeric>

namespace  {
  std::string interleave(const std::string & data , int rows, int cols);
}

std::string Encoder::encode(const std::string& data)
{
    unsigned char G1[8] = "\1\0\0\1\1\1\1";
    unsigned char G2[8] = "\1\1\0\1\1\0\1";

    std::string result(data.size() * 2, '0');
    std::string input("000000");
    input += data;
    const char * raw_data = input.data();


    unsigned char buffer[7]; 
    for (size_t i = 0; i < data.size(); ++i) {
        std::transform(raw_data + i, raw_data + i + 7,  G1, buffer, std::bit_and<char>());
        result[2*i] = std::accumulate(buffer, buffer + 7, '0', std::bit_xor<char>());
        
        std::transform(raw_data + i, raw_data + i + 7,  G2, buffer, std::bit_and<char>());
        result[2*i + 1] = std::accumulate(buffer, buffer + 7, '1', std::bit_xor<char>()); // Starts at '1' to act like a not
    }

    return result;
}

std::string Encoder::interleave_fnav(const std::string& data)
{
    assert(data.size() == 488);
    return interleave(data, 8, 61);
}

std::string Encoder::deinterleave_fnav(const std::string& data)
{
    assert(data.size() == 488);
    return interleave(data, 61, 8);
}

std::string Encoder::interleave_inav(const std::string& data)
{
    assert(data.size() == 240);
    return interleave(data, 8, 30);
}

std::string Encoder::deinterleave_inav(const std::string& data)
{
    assert(data.size() == 240);
    return interleave(data, 30, 8);
}

namespace  {
  std::string interleave(const std::string & data , int rows, int cols)
  {
    std::string result(data.size(), '0');
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // result[i][j] = input[j][i]
            result[i*cols + j] = data[j*rows + i];
        }
    }
    return result;
  }
}
