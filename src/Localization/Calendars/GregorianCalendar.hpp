#ifndef NB_SRC_LOCALIZATION_CALENDARS_GREGORIANCALENDAR_HPP
#define NB_SRC_LOCALIZATION_CALENDARS_GREGORIANCALENDAR_HPP

#include "ICalendar.hpp"
#include "Localization/Translation.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <ctime>

namespace Localization
{
    class GregorianCalendar final : public ICalendar
    {
    public:
        static constexpr std::string_view SYSTEM_NAME = "gregorian";

        static constexpr int32 DAYS_IN_WEEK = 7;
        static constexpr int32 MONTHS_IN_YEAR = 12;
        static constexpr int32 DAYS_IN_YEAR = 365;

        static constexpr int32 YEARS_IN_4_CYCLE = 4;
        static constexpr int32 YEARS_IN_100_CYCLE = 100;
        static constexpr int32 YEARS_IN_400_CYCLE = 400;

        static constexpr int32 DAYS_IN_4_CYCLE = 1461;   // 365 * 4 + 1
        static constexpr int32 DAYS_IN_100_CYCLE = 36524; // 365 * 100 + 24
        static constexpr int32 DAYS_IN_400_CYCLE = 146097;// 365 * 400 + 97

        static constexpr int64 UNIX_EPOCH_FIXED = 719163;
        static constexpr int64 SECONDS_PER_DAY = 86400;

        static constexpr std::array<int32, MONTHS_IN_YEAR> DAYS_PER_MONTH = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };

        static constexpr std::array<std::string_view, MONTHS_IN_YEAR> MONTHS_KEYS = {
            "Calendar.Gregorian.Month_01_Full",
            "Calendar.Gregorian.Month_02_Full",
            "Calendar.Gregorian.Month_03_Full",
            "Calendar.Gregorian.Month_04_Full",
            "Calendar.Gregorian.Month_05_Full",
            "Calendar.Gregorian.Month_06_Full",
            "Calendar.Gregorian.Month_07_Full",
            "Calendar.Gregorian.Month_08_Full",
            "Calendar.Gregorian.Month_09_Full",
            "Calendar.Gregorian.Month_10_Full",
            "Calendar.Gregorian.Month_11_Full",
            "Calendar.Gregorian.Month_12_Full"
        };

        static constexpr std::array<std::string_view, DAYS_IN_WEEK> WEEK_KEYS = {
            "Calendar.Gregorian.Weekday_01_Full",
            "Calendar.Gregorian.Weekday_02_Full",
            "Calendar.Gregorian.Weekday_03_Full",
            "Calendar.Gregorian.Weekday_04_Full",
            "Calendar.Gregorian.Weekday_05_Full",
            "Calendar.Gregorian.Weekday_06_Full",
            "Calendar.Gregorian.Weekday_07_Full"
        };

        std::string_view getSystemName() const noexcept override;

        int32 getDaysInWeek() const noexcept override;
        
        int32 getMonthsInYear(int32 year) const noexcept override;
        
        bool isLeapYear(int32 year) const noexcept override;
        
        int32 getDaysInMonth(int32 year, int32 month) const noexcept override;
        
        int64 dateToFixed(int32 year, int32 month, int32 day) const noexcept override;
        
        CalendarDate fixedToDate(int64 fixed) const noexcept override;
        
        CalendarDate timeToCalendarDate(time_t unixTime) const noexcept override;

        int32 getDayOfWeekIndex(int64 fixed) const noexcept override;

        std::string_view getMonthKey(int32 year, int32 month) const noexcept override;
        
        std::string getDayOfWeekKey(int32 index) const noexcept override;
        
    };
}

#endif
