#pragma once

#include <string>
#include <vector>
#include "rinex_record.pb.h"

class RinexManager
{
public:
    void load(const std::string &rinex_path);

    const std::vector<rinex_manager::RinexNavRecord> &get_rinex_records() const;

private:
    std::vector<rinex_manager::RinexNavRecord> rinex_records;
};