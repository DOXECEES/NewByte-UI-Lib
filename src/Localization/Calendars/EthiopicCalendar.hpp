#ifndef NB_SRC_LOCALIZATION_CALENDARS_ETHIOPICCALENDAR_HPP
#define NB_SRC_LOCALIZATION_CALENDARS_ETHIOPICCALENDAR_HPP

#include "ICalendar.hpp"
#include "Localization/Translation.hpp"
#include <Array.hpp>

namespace Localization
{
    class EthiopicCalendar final : public ICalendar
    {
    public:
        static constexpr std::string_view SYSTEM_NAME = "ethiopic";

        static constexpr int32 DAYS_IN_WEEK = 7;
        static constexpr int32 MONTHS_IN_YEAR = 13;
        static constexpr int32 DAYS_IN_MONTH = 30;
        static constexpr int32 DAYS_IN_YEAR = 365;
        static constexpr int32 LEAP_DAYS_IN_YEAR = 366;

        static constexpr int32 PAGUME_MONTH = 13;
        static constexpr int32 PAGUME_DAYS_COMMON = 5;
        static constexpr int32 PAGUME_DAYS_LEAP = 6;

        static constexpr int32 LEAP_CYCLE_YEARS = 4;
        static constexpr int32 LEAP_CYCLE_DAYS = 1461; // 365 * 4 + 1

        static constexpr int64 ETHIOPIC_EPOCH_FIXED = 2796;   // Fixed day
        static constexpr int64 UNIX_EPOCH_FIXED = 719163; // 1970-01-01 (Gregorian)
        static constexpr int64 SECONDS_PER_DAY = 86400;

        static constexpr nbstl::Array<std::string_view, MONTHS_IN_YEAR> MONTHS_KEYS = {
            "Calendar.Ethiopic.Month_01",
            "Calendar.Ethiopic.Month_02",
            "Calendar.Ethiopic.Month_03",
            "Calendar.Ethiopic.Month_04",
            "Calendar.Ethiopic.Month_05",
            "Calendar.Ethiopic.Month_06",
            "Calendar.Ethiopic.Month_07",
            "Calendar.Ethiopic.Month_08",
            "Calendar.Ethiopic.Month_09",
            "Calendar.Ethiopic.Month_10",
            "Calendar.Ethiopic.Month_11",
            "Calendar.Ethiopic.Month_12",
            "Calendar.Ethiopic.Month_13"
        };

        std::string_view getSystemName() const noexcept;

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
