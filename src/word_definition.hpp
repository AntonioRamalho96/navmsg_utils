#pragma once
#include <string>
#include <vector>

struct LineValues {
    std::string name;
    int n_bts;
    bool is_unsigned;
    double scale;
    double offset;
};

class WordDefinition {
public:
    std::string name;
    std::vector<LineValues> entries;

    static WordDefinition get_word_config(const std::string& file_path);
};
