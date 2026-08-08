#include "rinex_manager/rinex_reader.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <stdexcept>

static std::filesystem::path get_rinex_4_path()
{
    return std::filesystem::path(__FILE__).parent_path() / "test_files" / "rinex_4.rnx";
}


TEST(RinexReaterTests, GoodWeather)
{
    auto rr = RinexReader(get_rinex_4_path().string());
    ASSERT_TRUE(rr.next_record());
    ASSERT_EQ(rr.get_char(), ">");
    rr.skip_space();
    ASSERT_EQ(rr.get_char(3), "EPH");
    rr.skip_space();
    ASSERT_EQ(rr.get_char(), "G");
    ASSERT_EQ(rr.get_int(2), 4);
    rr.skip_space();
    ASSERT_EQ(rr.get_char(4), "LNAV");


    ASSERT_TRUE(rr.next_line());
    ASSERT_TRUE(rr.next_line());
    rr.skip_space(4);
    ASSERT_EQ(rr.get_19_char_int(), 39);
    ASSERT_DOUBLE_EQ(rr.get_19_char_double(), 8.781250000000e+00);
    ASSERT_DOUBLE_EQ(rr.get_19_char_double(), 4.580905098799e-09);
    ASSERT_DOUBLE_EQ(rr.get_19_char_double(), -1.604131160810e+00);

    ASSERT_TRUE(rr.next_record());
    ASSERT_EQ(rr.get_char(), ">");
    ASSERT_TRUE(rr.next_record());
    ASSERT_FALSE(rr.next_record()); // No more records
    ASSERT_FALSE(rr.next_line()); // No more lines
}

TEST(RinexReaterTests, BadWeatherNoFile)
{
    EXPECT_THROW(RinexReader("bad_name"), std::runtime_error);
}

TEST(RinexReaterTests, BadWeatherNotSpace)
{
    auto rr = RinexReader(RINEX_4_PATH);
    rr.next_record();
    rr.get_char(); // skip ">"
    EXPECT_THROW(rr.skip_space(2), std::invalid_argument);

    rr = RinexReader(RINEX_4_PATH);
    rr.next_record();
    rr.get_char(); // skip ">"
    EXPECT_NO_THROW(rr.skip_space());
    EXPECT_THROW(rr.skip_space(), std::invalid_argument);
}

TEST(RinexReaterTests, BadWeatherNotInteger)
{
    auto rr = RinexReader(RINEX_4_PATH);
    rr.next_record();
    rr.get_char(6); // skip "> EPH "
    EXPECT_THROW(rr.get_int(3), std::invalid_argument); // G04 is not an integer
}

TEST(RinexReaterTests, BadWeatherNotDouble)
{
    auto rr = RinexReader(RINEX_4_PATH);
    rr.next_record();
    rr.get_char(6); // skip "> EPH "
    EXPECT_THROW(rr.get_19_char_double(), std::invalid_argument); // " G04 LNAV" is not a double
}
