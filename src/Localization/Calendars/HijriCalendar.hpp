#ifndef NB_SRC_LOCALIZATION_CALENDARS_HIJRICALENDAR_HPP
#define NB_SRC_LOCALIZATION_CALENDARS_HIJRICALENDAR_HPP

#include "ICalendar.hpp"
#include "Localization/Translation.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <ctime>

namespace Localization
{
    class HijriCalendar final : public ICalendar
    {
    public:
        static constexpr std::string_view   SYSTEM_NAME = "hijri";
        static constexpr int32              DAYS_IN_WEEK = 7;
        static constexpr int32              MONTHS_IN_YEAR = 12;

        static constexpr std::array<std::string_view, MONTHS_IN_YEAR> MONTHS_KEYS = {
            "Calendar.Hijri.Month_01", // Muharram
            "Calendar.Hijri.Month_02", // Safar
            "Calendar.Hijri.Month_03", // Rabi' al-awwal
            "Calendar.Hijri.Month_04", // Rabi' al-thani
            "Calendar.Hijri.Month_05", // Jumada al-ula
            "Calendar.Hijri.Month_06", // Jumada al-akhira
            "Calendar.Hijri.Month_07", // Rajab
            "Calendar.Hijri.Month_08", // Sha'ban
            "Calendar.Hijri.Month_09", // Ramadan
            "Calendar.Hijri.Month_10", // Shawwal
            "Calendar.Hijri.Month_11", // Dhu al-Qi'dah
            "Calendar.Hijri.Month_12"  // Dhu al-Hijjah
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

        std::string_view getSystemName() const noexcept override { return SYSTEM_NAME; }
        int32 getDaysInWeek() const noexcept override { return DAYS_IN_WEEK; }
        int32 getMonthsInYear(int year) const noexcept override { return MONTHS_IN_YEAR; }

        bool isLeapYear(int32 year) const noexcept override
        {
            return ((11 * (int64)year + 14) % 30) < 11;
        }

        int32 getDaysInMonth(int32 year, int32 month) const noexcept override
        {
            if (month < 1 || month > 12) return 0;

            if (month == 12) return isLeapYear(year) ? 30 : 29;
            return (month % 2 != 0) ? 30 : 29;
        }

        int64 dateToFixed(int32 year, int32 month, int32 day) const noexcept override
        {
            int64 y = year;
            return day                      
                + (month - 1) * 29          
                + month / 2                 
                + (y - 1) * 354             
                + (11 * y + 3) / 30         
                + 227014;                   
        }

        CalendarDate fixedToDate(int64 fixed) const noexcept override
        {
            int64 days = fixed - 227015;
            int32 year = static_cast<int32>((30 * days + 10646) / 10631);

            // Корректировка года
            if (dateToFixed(year + 1, 1, 1) <= fixed) year++;

            int64 dayOfYear = fixed - dateToFixed(year, 1, 1) + 1;
            int32 month = 1;

            while (month < 12) {
                int32 dim = getDaysInMonth(year, month);
                if (dayOfYear <= dim) break;
                dayOfYear -= dim;
                month++;
            }

            return { year, month, static_cast<int>(dayOfYear) };
        }

        CalendarDate timeToCalendarDate(time_t unixTime) const noexcept override
        {
            int64 fixedDay = 719163 + (static_cast<int64>(unixTime) / 86400);
            return fixedToDate(fixedDay);
        }

        int32 getDayOfWeekIndex(int64 fixed) const noexcept override
        {
            return static_cast<int32>((fixed + 6) % 7);
        }

        std::string_view getMonthKey(int32 year, int32 month) const noexcept override
        {
            if (month < 1 || month > 12) return {};
            return Translation::fromKey(MONTHS_KEYS[month - 1].data());
        }

        std::string getDayOfWeekKey(int32 index) const noexcept override
        {
            if (index < 0 || index >= 7) return "ddd";
            return Translation::fromKey(WEEK_KEYS[index].data());
        }
    };
}

#endif