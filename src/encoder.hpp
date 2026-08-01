#include <string>

class Encoder {
public:
    static std::string encode(const std::string& data);
    static std::string interleave_inav(const std::string& data);
    static std::string interleave_fnav(const std::string& data);
    static std::string deinterleave_fnav(const std::string& data);
    static std::string deinterleave_inav(const std::string& data);
};
