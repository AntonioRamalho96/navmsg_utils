#include "rinex_manager/rinex_manager.hpp"
#include <filesystem>
#include <gtest/gtest.h>

static std::filesystem::path get_rinex_4_path()
{
    return std::filesystem::path(__FILE__).parent_path() / "test_files" / "rinex_4.rnx";
}

// ── helpers ──────────────────────────────────────────────────────────────────
using namespace rinex_manager;

static RinexManager load_rinex4()
{
    RinexManager mgr;
    mgr.load(get_rinex_4_path().string());
    return mgr;
}

static RinexNavRecordType get_type(const RinexNavRecord &r)
{
    switch (r.record_case()) {
        case RinexNavRecord::kGps:     return rinex_manager::GPS_LNAV;
        case RinexNavRecord::kGalileo: return r.galileo().type();
        default:
            return static_cast<RinexNavRecordType>(-1);
    }
}

// ── GPS LNAV ─────────────────────────────────────────────────────────────────

TEST(RinexManagerTests, GpsLnavRecord_GoodWeather)
{
    auto mgr = load_rinex4();
    const auto &records = mgr.get_rinex_records();

    ASSERT_GE(records.size(), 1u);
    ASSERT_EQ(get_type(records[0]), rinex_manager::GPS_LNAV);

    const auto &r   = records[0];
    const auto &gps = r.gps();

    // SV id / epoch
    EXPECT_EQ(r.svid(),   1);
    EXPECT_EQ(r.year(),   2014);
    EXPECT_EQ(r.month(),  8);
    EXPECT_EQ(r.day(),    8);
    EXPECT_EQ(r.hour(),   2);
    EXPECT_EQ(r.minute(), 0);
    EXPECT_DOUBLE_EQ(r.second(), 0.0);

    // SV clock
    EXPECT_DOUBLE_EQ(r.sv_clock_bias(),       1.921365037560e-05);
    EXPECT_DOUBLE_EQ(r.sv_clock_drift(),      6.821210263297e-13);
    EXPECT_DOUBLE_EQ(r.sv_clock_drift_rate(), 0.0);

    // Broadcast orbit 1
    EXPECT_EQ(r.iode(), 39);
    EXPECT_DOUBLE_EQ(r.c_rs(),    8.781250000000e+00);
    EXPECT_DOUBLE_EQ(r.delta_n(), 4.580905098799e-09);
    EXPECT_DOUBLE_EQ(r.m0(),     -1.604131160810e+00);

    // Broadcast orbit 2
    EXPECT_DOUBLE_EQ(r.c_uc(),   2.644956111908e-07);
    EXPECT_DOUBLE_EQ(r.e(),      3.353283973411e-03);
    EXPECT_DOUBLE_EQ(r.c_us(),   1.009553670883e-05);
    EXPECT_DOUBLE_EQ(r.sqrt_a(), 5.153675058365e+03);

    // Broadcast orbit 3
    EXPECT_EQ(r.toe(), 439200);
    EXPECT_DOUBLE_EQ(r.c_ic(),   -5.587935447693e-09);
    EXPECT_DOUBLE_EQ(r.omega0(), -2.949709776143e+00);
    EXPECT_DOUBLE_EQ(r.c_is(),   -5.401670932770e-08);

    // Broadcast orbit 4
    EXPECT_DOUBLE_EQ(r.i0(),        9.615440229110e-01);
    EXPECT_DOUBLE_EQ(r.c_rc(),      1.844062500000e+02);
    EXPECT_DOUBLE_EQ(r.omega(),     3.645818649724e-01);
    EXPECT_DOUBLE_EQ(r.omega_dot(),-7.927473068216e-09);

    // Broadcast orbit 5
    EXPECT_DOUBLE_EQ(r.i_dot(),         6.785996949726e-10);
    EXPECT_EQ(gps.codes_on_l2(), 1);
    EXPECT_EQ(gps.gps_week(),    1804);

    // Broadcast orbit 6
    EXPECT_DOUBLE_EQ(gps.sv_accuracy(), 2.0);
    EXPECT_EQ(gps.sv_health(), 0);
    EXPECT_DOUBLE_EQ(gps.t_gd(), 5.587935447693e-09);
    EXPECT_EQ(gps.iodc(), 39);

    // Broadcast orbit 7
    EXPECT_EQ(r.t_tm(),          432000);
    EXPECT_EQ(gps.fit_interval(), 4);
}

// ── Galileo INAV ─────────────────────────────────────────────────────────────

TEST(RinexManagerTests, GalileoInavRecord_GoodWeather)
{
    auto mgr = load_rinex4();
    const auto &records = mgr.get_rinex_records();

    ASSERT_GE(records.size(), 2u);
    ASSERT_EQ(get_type(records[1]), rinex_manager::GALILEO_INAV);

    const auto &r   = records[1];
    const auto &gal = r.galileo();

    // SV id / epoch
    EXPECT_EQ(r.svid(),   12);
    EXPECT_EQ(r.year(),   2020);
    EXPECT_EQ(r.month(),  9);
    EXPECT_EQ(r.day(),    15);
    EXPECT_EQ(r.hour(),   0);
    EXPECT_EQ(r.minute(), 40);
    EXPECT_DOUBLE_EQ(r.second(), 0.0);

    // SV clock
    EXPECT_DOUBLE_EQ(r.sv_clock_bias(),       5.605182959698e-03);
    EXPECT_DOUBLE_EQ(r.sv_clock_drift(),     -1.881517164293e-11);
    EXPECT_DOUBLE_EQ(r.sv_clock_drift_rate(), 0.0);

    // Broadcast orbit 1
    EXPECT_EQ(r.iode(), 36);
    EXPECT_DOUBLE_EQ(r.c_rs(),    1.090625000000e+02);
    EXPECT_DOUBLE_EQ(r.delta_n(), 2.811188525857e-09);
    EXPECT_DOUBLE_EQ(r.m0(),     -2.481435854929e+00);

    // Broadcast orbit 2
    EXPECT_DOUBLE_EQ(r.c_uc(),   5.209818482399e-06);
    EXPECT_DOUBLE_EQ(r.e(),      1.468013506383e-04);
    EXPECT_DOUBLE_EQ(r.c_us(),   1.532956957817e-06);
    EXPECT_DOUBLE_EQ(r.sqrt_a(), 5.440609727859e+03);

    // Broadcast orbit 3
    EXPECT_EQ(r.toe(), 175200);
    EXPECT_DOUBLE_EQ(r.c_ic(),  -1.676380634308e-08);
    EXPECT_DOUBLE_EQ(r.omega0(), 8.103706855689e-01);
    EXPECT_DOUBLE_EQ(r.c_is(),   7.450580596924e-09);

    // Broadcast orbit 4
    EXPECT_DOUBLE_EQ(r.i0(),        9.891660140720e-01);
    EXPECT_DOUBLE_EQ(r.c_rc(),      3.219375000000e+02);
    EXPECT_DOUBLE_EQ(r.omega(),     5.171049929386e-01);
    EXPECT_DOUBLE_EQ(r.omega_dot(),-5.815956543649e-09);

    // Broadcast orbit 5
    EXPECT_DOUBLE_EQ(r.i_dot(), 2.982267080537e-10);
    EXPECT_EQ(gal.data_source(), rinex_manager::INAV_MERGED);
    EXPECT_EQ(gal.gal_week(), 2123);

    // Broadcast orbit 6
    EXPECT_DOUBLE_EQ(gal.bgd_e5a_e1(), -1.303851604462e-08);
    EXPECT_DOUBLE_EQ(gal.bgd_e5b_e1(), -1.280568540096e-08);

    // Broadcast orbit 7
    EXPECT_EQ(r.t_tm(), 176434);
}

// ── Galileo FNAV ─────────────────────────────────────────────────────────────

TEST(RinexManagerTests, GalileoFnavRecord_GoodWeather)
{
    auto mgr = load_rinex4();
    const auto &records = mgr.get_rinex_records();

    ASSERT_GE(records.size(), 3u);
    ASSERT_EQ(get_type(records[2]), rinex_manager::GALILEO_FNAV);

    const auto &r   = records[2];
    const auto &gal = r.galileo();

    // SV id / epoch
    EXPECT_EQ(r.svid(),   11);
    EXPECT_EQ(r.year(),   2020);
    EXPECT_EQ(r.month(),  9);
    EXPECT_EQ(r.day(),    15);
    EXPECT_EQ(r.hour(),   23);
    EXPECT_EQ(r.minute(), 30);
    EXPECT_DOUBLE_EQ(r.second(), 0.0);

    // SV clock
    EXPECT_DOUBLE_EQ(r.sv_clock_bias(),       5.537368822843e-03);
    EXPECT_DOUBLE_EQ(r.sv_clock_drift(),      2.744400262600e-10);
    EXPECT_DOUBLE_EQ(r.sv_clock_drift_rate(), 0.0);

    // Broadcast orbit 1
    EXPECT_EQ(r.iode(), 45);
    EXPECT_DOUBLE_EQ(r.c_rs(),    1.730312500000e+02);
    EXPECT_DOUBLE_EQ(r.delta_n(), 2.871548182937e-09);
    EXPECT_DOUBLE_EQ(r.m0(),     -1.103934352668e-01);

    // Broadcast orbit 2
    EXPECT_DOUBLE_EQ(r.c_uc(),   8.083879947662e-06);
    EXPECT_DOUBLE_EQ(r.e(),      2.968260087073e-04);
    EXPECT_DOUBLE_EQ(r.c_us(),   3.607943654060e-06);
    EXPECT_DOUBLE_EQ(r.sqrt_a(), 5.440606000900e+03);

    // Broadcast orbit 3
    EXPECT_EQ(r.toe(), 257400);
    EXPECT_DOUBLE_EQ(r.c_ic(),  -5.774199962616e-08);
    EXPECT_DOUBLE_EQ(r.omega0(), 8.098963343817e-01);
    EXPECT_DOUBLE_EQ(r.c_is(),  -1.005828380585e-07);

    // Broadcast orbit 4
    EXPECT_DOUBLE_EQ(r.i0(),        9.891873024559e-01);
    EXPECT_DOUBLE_EQ(r.c_rc(),      2.774062500000e+02);
    EXPECT_DOUBLE_EQ(r.omega(),     1.248848716430e+00);
    EXPECT_DOUBLE_EQ(r.omega_dot(),-5.818456647788e-09);

    // Broadcast orbit 5
    EXPECT_DOUBLE_EQ(r.i_dot(), 5.564517498775e-10);
    EXPECT_EQ(gal.data_source(), rinex_manager::FNAV);
    EXPECT_EQ(gal.gal_week(), 2123);

    // Broadcast orbit 6
    EXPECT_DOUBLE_EQ(gal.bgd_e5a_e1(), -1.583248376846e-08);
    EXPECT_DOUBLE_EQ(gal.bgd_e5b_e1(),  0.0);

    // Broadcast orbit 7
    EXPECT_EQ(r.t_tm(), 258100);
}
