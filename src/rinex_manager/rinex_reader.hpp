#pragma once

#include <fstream>
#include <string>




class RinexReader
{  
/**
    * @brief  Class to read rinex files
    *
    * Notes:
    * - All parsing functions advance the internal offset by the number of
    *   characters they consume.
    */
public:
    struct ReadPosition
    {
        std::string file_path;
        int line_nr{0};
        int offset;
        std::string to_str(int offset_sub = 0) const;
    };

    RinexReader(const std::string &rinex_file);

    /**
    * @brief Read the next line from the input file.
    * @return true if a new line was successfully read; false if end-of-file
    * or an unrecoverable read error occurred.
    */
    bool next_line();

    /**
    * bool next_record()
    * @brief Advance to the next RINEX record.
    * @return true if a record was found and loaded; false if there are no more
    * records (end-of-file) or if an error prevented reading the next record.
    */
    bool next_record();

    /**
    * @brief Advance the internal offset by skipping whitespace(s).
    * @param n Number of whitespace characters to skip (default 1).
    * @throws InvaludValue if the skipped chars are not spaces
    */
    void skip_space(size_t n = 1);

    /**
    * @brief Extract a raw substring of length n from the current line.
    * @param n Number of characters to extract (default 1).
    * @return Extracted substring, with size n or until end of line
    */
    std::string get_char(size_t n = 1);

    /**
    * @brief Parse a fixed-width integer field of width n from the current line.
    * @param n Field width (number of characters to consume).
    * @return Parsed integer value.
    * @throws Invalid if the field does not contain a valid positive
    * integer representation.
    */
    int get_int(size_t n);

    /**
    * @brief Parse a double value from a 19-character RINEX-style field.
    * @return Parsed double value.
    * @throws InvalidValue if the field is not a well formed RINEX scientific number
    */
    double get_19_char_double();

    /**
    * @brief Parse a double value from a 19-character RINEX-style field.
    * @return Closest integer to the parsed double value.
    * @throws InvalidValue if the field is not a well formed RINEX scientific number
    */    
    int get_19_char_int();

    /**
     * @brief Get the integer represented as 10 bits
     * 
     * @return std::array<bool, 10> 10 bits, the first one (pos=0) represents the LSB
     */
    std::array<bool, 10> get_19_char_bits();

    ReadPosition get_pos() const;

    

private:
    std::string current_line;
    std::ifstream file;
    ReadPosition read_pos{};
};