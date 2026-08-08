#include "rinex_manager.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <array>

#include "rinex_manager/constellation_type.hpp"
#include "rinex_manager/rinex_reader.hpp"

using namespace rinex_manager;

namespace {
    RinexNavRecord process_record(RinexReader *reader);
}

void RinexManager::load(const std::string &rinex_path)
{
    RinexReader reader{rinex_path};

    while (reader.next_record())
    {
        try {
            rinex_records.emplace_back(process_record(&reader));
        } catch (const std::exception &e) {
            std::ostringstream message;
            message << "Error loading RINEX around: " << reader.get_pos().to_str() << " - " << e.what();
            throw std::runtime_error(message.str());
        }
    }
}

const std::vector<RinexNavRecord> &RinexManager::get_rinex_records() const
{
    return rinex_records;
}

namespace {

    void skip_expect(RinexReader *reader, const std::string &expected)
    {
        if (reader->get_char(expected.size()) != expected)
            throw std::invalid_argument("Expected \"" + expected + "\" at " + reader->get_pos().to_str());
    }

    void skip_record_start(RinexReader *reader)
    {
        skip_expect(reader, ">");
        reader->skip_space();
        skip_expect(reader, "EPH");
        reader->skip_space();
    }

    ConstellationType to_constellation_type(char c)
    {
        switch (c) {
            case 'G': return ConstellationType::GPS;
            case 'E': return ConstellationType::GALILEO;
            default:
                std::cout << "Unknown constellation type: " << c << std::endl;
                return ConstellationType::OTHER;
        }
    }

    void parse_clock(RinexNavRecord *r, RinexReader *reader)
    {
        r->set_year  (reader->get_int(4)); reader->skip_space();
        r->set_month (reader->get_int(2)); reader->skip_space();
        r->set_day   (reader->get_int(2)); reader->skip_space();
        r->set_hour  (reader->get_int(2)); reader->skip_space();
        r->set_minute(reader->get_int(2)); reader->skip_space();
        r->set_second(reader->get_int(2));
    }

    GalileoDataSource parse_inav_data_source(const std::array<bool, 10> &bits)
    {
        if (bits[1])
            throw std::invalid_argument("INAV record cannot have F/NAV bit set");
        if (!bits[0] && !bits[2])
            throw std::invalid_argument("INAV record must have one of the I/NAV bits set");

        if (bits[0] && bits[2])  return rinex_manager::INAV_MERGED;
        if (bits[0] && !bits[2]) return rinex_manager::INAV_E1;
        if (!bits[0] && bits[2]) return rinex_manager::INAV_E5;

        throw std::logic_error("Unexpected error parsing INAV data source");
    }

    GalileoDataSource parse_fnav_data_source(const std::array<bool, 10> &bits)
    {
        if (bits[0] || bits[2])
            throw std::invalid_argument("F/NAV record cannot have I/NAV bits set");
        if (!bits[1])
            throw std::invalid_argument("F/NAV record must have the F/NAV bit set");
        return rinex_manager::FNAV;
    }

    GalileoDataSource parse_data_source(const std::array<bool, 10> &bits, bool is_inav)
    {
        return is_inav ? parse_inav_data_source(bits) : parse_fnav_data_source(bits);
    }

    RinexNavRecord parse_galileo_record_body(RinexReader *reader, bool is_inav)
    {
        RinexNavRecord r;
        auto *gal = r.mutable_galileo();

        // line 1
        reader->next_line();
        skip_expect(reader, "E");
        r.set_svid(reader->get_int(2));
        reader->skip_space();
        parse_clock(&r, reader);
        r.set_sv_clock_bias      (reader->get_19_char_double());
        r.set_sv_clock_drift     (reader->get_19_char_double());
        r.set_sv_clock_drift_rate(reader->get_19_char_double());

        // line 2
        reader->next_line(); reader->skip_space(4);
        r.set_iode   (reader->get_19_char_int());
        r.set_c_rs   (reader->get_19_char_double());
        r.set_delta_n(reader->get_19_char_double());
        r.set_m0     (reader->get_19_char_double());

        // line 3
        reader->next_line(); reader->skip_space(4);
        r.set_c_uc  (reader->get_19_char_double());
        r.set_e     (reader->get_19_char_double());
        r.set_c_us  (reader->get_19_char_double());
        r.set_sqrt_a(reader->get_19_char_double());

        // line 4
        reader->next_line(); reader->skip_space(4);
        r.set_toe   (reader->get_19_char_int());
        r.set_c_ic  (reader->get_19_char_double());
        r.set_omega0(reader->get_19_char_double());
        r.set_c_is  (reader->get_19_char_double());

        // line 5
        reader->next_line(); reader->skip_space(4);
        r.set_i0       (reader->get_19_char_double());
        r.set_c_rc     (reader->get_19_char_double());
        r.set_omega    (reader->get_19_char_double());
        r.set_omega_dot(reader->get_19_char_double());

        // line 6
        reader->next_line(); reader->skip_space(4);
        r.set_i_dot(reader->get_19_char_double());
        gal->set_data_source(parse_data_source(reader->get_19_char_bits(), is_inav));
        gal->set_gal_week(reader->get_19_char_int());

        // line 7
        reader->next_line(); reader->skip_space(4);
        reader->get_19_char_int();    // SISA – ignored
        reader->get_19_char_bits();   // SV health – ignored
        gal->set_bgd_e5a_e1(reader->get_19_char_double());
        gal->set_bgd_e5b_e1(reader->get_19_char_double());

        // line 8
        reader->next_line(); reader->skip_space(4);
        r.set_t_tm(reader->get_19_char_int());

        // tag the record type inside the galileo sub-message
        gal->set_type(is_inav ? rinex_manager::GALILEO_INAV : rinex_manager::GALILEO_FNAV);

        return r;
    }

    RinexNavRecord parse_galileo_record(RinexReader *reader)
    {
        std::string nav_msg_type = reader->get_char(4);
        if (nav_msg_type == "INAV") return parse_galileo_record_body(reader, true);
        if (nav_msg_type == "FNAV") return parse_galileo_record_body(reader, false);
        throw std::invalid_argument("Unknown Galileo nav message type: " + nav_msg_type);
    }

    RinexNavRecord parse_gps_lnav(RinexReader *reader)
    {
        RinexNavRecord r;
        auto *gps = r.mutable_gps();

        // line 1
        reader->next_line();
        skip_expect(reader, "G");
        r.set_svid(reader->get_int(2));
        reader->skip_space();
        parse_clock(&r, reader);
        r.set_sv_clock_bias      (reader->get_19_char_double());
        r.set_sv_clock_drift     (reader->get_19_char_double());
        r.set_sv_clock_drift_rate(reader->get_19_char_double());

        // line 2
        reader->next_line(); reader->skip_space(4);
        r.set_iode   (reader->get_19_char_int());
        r.set_c_rs   (reader->get_19_char_double());
        r.set_delta_n(reader->get_19_char_double());
        r.set_m0     (reader->get_19_char_double());

        // line 3
        reader->next_line(); reader->skip_space(4);
        r.set_c_uc  (reader->get_19_char_double());
        r.set_e     (reader->get_19_char_double());
        r.set_c_us  (reader->get_19_char_double());
        r.set_sqrt_a(reader->get_19_char_double());

        // line 4
        reader->next_line(); reader->skip_space(4);
        r.set_toe   (reader->get_19_char_int());
        r.set_c_ic  (reader->get_19_char_double());
        r.set_omega0(reader->get_19_char_double());
        r.set_c_is  (reader->get_19_char_double());

        // line 5
        reader->next_line(); reader->skip_space(4);
        r.set_i0       (reader->get_19_char_double());
        r.set_c_rc     (reader->get_19_char_double());
        r.set_omega    (reader->get_19_char_double());
        r.set_omega_dot(reader->get_19_char_double());

        // line 6
        reader->next_line(); reader->skip_space(4);
        r.set_i_dot(reader->get_19_char_double());
        gps->set_codes_on_l2(reader->get_19_char_int());
        gps->set_gps_week   (reader->get_19_char_int());
        reader->get_19_char_double(); // L2 P flag spare

        // line 7
        reader->next_line(); reader->skip_space(4);
        gps->set_sv_accuracy (reader->get_19_char_double());
        gps->set_sv_health   (reader->get_19_char_int());
        gps->set_t_gd        (reader->get_19_char_double());
        gps->set_iodc        (reader->get_19_char_int());

        // line 8
        reader->next_line(); reader->skip_space(4);
        r.set_t_tm          (reader->get_19_char_int());
        gps->set_fit_interval(reader->get_19_char_int());

        return r;
    }

    RinexNavRecord parse_gps_record(RinexReader *reader)
    {
        std::string nav_msg_type = reader->get_char(4);
        if (nav_msg_type == "LNAV") return parse_gps_lnav(reader);
        throw std::invalid_argument("Unknown GPS nav message type: " + nav_msg_type);
    }

    RinexNavRecord process_record(RinexReader *reader)
    {
        skip_record_start(reader);

        auto constellation_type = to_constellation_type(reader->get_char()[0]);
        reader->get_int(2); // skip svid (re-parsed per constellation)
        reader->skip_space();

        switch (constellation_type) {
            case ConstellationType::GPS:     return parse_gps_record(reader);
            case ConstellationType::GALILEO: return parse_galileo_record(reader);
            default:
                std::cout << "Skipping unknown constellation type" << std::endl;
                return {};
        }
    }

} // namespace
