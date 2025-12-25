#ifndef NB_SRC_LOCALIZATION_CALENDARS_PERSIANCALENDAR_HPP
#define NB_SRC_LOCALIZATION_CALENDARS_PERSIANCALENDAR_HPP

#include "ICalendar.hpp"
#include "Localization/Translation.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <ctime>

namespace Localization
{
    class PersianCalendar final : public ICalendar
    {
    private:
        // Вспомогательные математические функции для корректной работы с отрицательными датами
        static inline int64_t mathMod(int64_t a, int64_t b) noexcept {
            int64_t r = a % b;
            return r < 0 ? r + b : r;
        }

        static inline int64_t mathFloorDiv(int64_t a, int64_t b) noexcept {
            if (a >= 0) return a / b;
            return (a - b + 1) / b;
        }

    public:
        static constexpr std::string_view   SYSTEM_NAME = "persian";
        static constexpr int32              DAYS_IN_WEEK = 7;
        static constexpr int32              MONTHS_IN_YEAR = 12;

        // Эпоха: 19 марта 622 года н.э. (Григорианский) = Fixed Day 226896
        static constexpr int64 PERSIAN_EPOCH = 226896;

        static constexpr std::array<std::string_view, MONTHS_IN_YEAR> MONTHS_KEYS = {
            "Calendar.Persian.Month_01_Full", // Farvardin
            "Calendar.Persian.Month_02_Full", // Ordibehesht
            "Calendar.Persian.Month_03_Full", // Khordad
            "Calendar.Persian.Month_04_Full", // Tir
            "Calendar.Persian.Month_05_Full", // Mordad
            "Calendar.Persian.Month_06_Full", // Shahrivar
            "Calendar.Persian.Month_07_Full", // Mehr
            "Calendar.Persian.Month_08_Full", // Aban
            "Calendar.Persian.Month_09_Full", // Azar
            "Calendar.Persian.Month_10_Full", // Dey
            "Calendar.Persian.Month_11_Full", // Bahman
            "Calendar.Persian.Month_12_Full"  // Esfand
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
            // Алгоритмический цикл (33 года): високосные 1, 5, 9, 13, 17, 22, 26, 30
            return mathMod((static_cast<int64>(year) + 38) * 31, 128) < 31;
        }

        int32 getDaysInMonth(int32 year, int32 month) const noexcept override
        {
            if (month < 1 || month > 12) return 0;
            if (month <= 6) return 31;
            if (month <= 11) return 30;
            return isLeapYear(year) ? 30 : 29;
        }

        int64 dateToFixed(int32 year, int32 month, int32 day) const noexcept override
        {
            const int64 y = static_cast<int64>(year);
            const int64 epBase = y - (y >= 0 ? 474 : 473);
            const int64 epYear = 474 + mathMod(epBase, 2820);

            return day +
                ((month <= 7) ? ((month - 1) * 31) : (((month - 1) * 30) + 6)) +
                mathFloorDiv(epYear * 682 - 110, 2816) +
                (epYear - 1) * 365 +
                mathFloorDiv(epBase, 2820) * 1029983 +
                (PERSIAN_EPOCH - 1);
        }

        CalendarDate fixedToDate(int64 fixed) const noexcept override
        {
            const int64 offset = fixed - dateToFixed(475, 1, 1);
            const int64 cycle = mathFloorDiv(offset, 1029983);
            const int64 remainder = mathMod(offset, 1029983);

            int64 yCycle;
            if (remainder == 1029982) {
                yCycle = 2820;
            }
            else {
                int64 aux = 2816 * remainder + 1031337;
                yCycle = mathFloorDiv(aux, 1028160);
            }

            int32 year = static_cast<int32>(474 + 2820 * cycle + yCycle);
            if (year <= 0) year--; // Обработка отсутствия нулевого года

            const int64 dayOfYear = fixed - dateToFixed(year, 1, 1) + 1;
            int32 month, day;

            if (dayOfYear <= 186) {
                month = static_cast<int32>(mathFloorDiv(dayOfYear - 1, 31) + 1);
                day = static_cast<int32>(mathMod(dayOfYear - 1, 31) + 1);
            }
            else {
                month = static_cast<int32>(mathFloorDiv(dayOfYear - 187, 30) + 7);
                day = static_cast<int32>(mathMod(dayOfYear - 187, 30) + 1);
            }

            return { year, month, day };
        }

        CalendarDate timeToCalendarDate(time_t unixTime) const noexcept override
        {
            // Unix Time 1970-01-01 (Gregorian) = Fixed Day 719163
            int64 fixedDay = 719163 + mathFloorDiv(static_cast<int64>(unixTime), 86400);
            return fixedToDate(fixedDay);
        }

        int32 getDayOfWeekIndex(int64 fixed) const noexcept override
        {
            // Общий цикл недель для всех календарей
            // fixed=1 (1.1.0001 Gregorian) был Понедельником.
            // Приводим к индексу Monday = 0
            return static_cast<int32>(mathMod(fixed + 5, 7));
        }

        std::string_view getMonthKey(int32 year, int32 month) const noexcept override
        {
            return (month >= 1 && month <= 12) ? Translation::fromKey(MONTHS_KEYS[month - 1].data()) : std::string_view{};
        }

        std::string getDayOfWeekKey(int32 index) const noexcept override
        {
            return (index >= 0 && index < 7) ? Translation::fromKey(WEEK_KEYS[index].data()) : "ddd";
        }
    };
}

#endif