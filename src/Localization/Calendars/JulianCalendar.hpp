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
        /**
         * Математически корректное деление (floor), необходимое для календарных расчетов.
         * В отличие от оператора '/', который просто отбрасывает дробную часть,
         * эта функция всегда округляет результат вниз.
         */
        static inline int64_t floorDiv(int64_t a, int64_t b) noexcept {
            return (a >= 0) ? (a / b) : ((a - b + 1) / b);
        }

        /**
         * Математически корректный остаток от деления.
         * Всегда возвращает положительное число в диапазоне [0, b-1].
         */
        static inline int64_t floorMod(int64_t a, int64_t b) noexcept  {
            int64_t r = a % b;
            return (r < 0) ? (r + b) : r;
        }

    public:
        static constexpr std::string_view   SYSTEM_NAME = "persian";
        static constexpr int32              DAYS_IN_WEEK = 7;
        static constexpr int32              MONTHS_IN_YEAR = 12;

        /**
         * Эпоха Персидского календаря (1 Фарвардина 1 года):
         * Соответствует 19 марта 622 года н.э. по Григорианскому календарю.
         * В системе абсолютных дней (Fixed Days / Rata Die) это день № 226,896.
         */
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
            "Calendar.Gregorian.Weekday_01_Full", // Monday
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

        /**
         * Расчет високосного года по 33-летнему циклу.
         * Это обеспечивает точность в 1 день за 3800 лет.
         */
        bool isLeapYear(int32 year) const noexcept override
        {
            return floorMod((static_cast<int64>(year) + 38) * 31, 128) < 31;
        }

        int32 getDaysInMonth(int32 year, int32 month) const noexcept override
        {
            if (month < 1 || month > 12) return 0;
            if (month <= 6) return 31;        // Первое полугодие
            if (month <= 11) return 30;       // Второе полугодие
            return isLeapYear(year) ? 30 : 29; // Эсфанд
        }

        /**
         * Перевод даты Персидского календаря в абсолютное число дней (RD).
         */
        int64 dateToFixed(int32 year, int32 month, int32 day) const noexcept override
        {
            const int64 y = (year > 0) ? (year - 1) : year;

            // Количество дней от эпохи до начала года
            int64 fixed = PERSIAN_EPOCH - 1 + 365 * y + floorDiv(31 * y + 38, 128);

            // Дни внутри года
            if (month <= 7) {
                fixed += (month - 1) * 31;
            }
            else {
                fixed += (month - 1) * 30 + 6;
            }

            return fixed + day;
        }

        /**
         * Перевод абсолютного числа дней (RD) в дату Персидского календаря.
         */
        CalendarDate fixedToDate(int64 fixed) const noexcept override
        {
            const int64 daysSinceEpoch = fixed - PERSIAN_EPOCH;

            // Приблизительный расчет года
            int32 year = static_cast<int32>(floorDiv(128 * daysSinceEpoch + 468, 46751)) + 1;

            // Уточнение года
            int64 yearStart = dateToFixed(year, 1, 1);
            if (yearStart > fixed) {
                year--;
                yearStart = dateToFixed(year, 1, 1);
            }

            int64 dayOfYear = fixed - yearStart + 1;
            int32 month, day;

            if (dayOfYear <= 186) {
                month = static_cast<int32>(floorDiv(dayOfYear - 1, 31) + 1);
                day = static_cast<int32>(floorMod(dayOfYear - 1, 31) + 1);
            }
            else {
                month = static_cast<int32>(floorDiv(dayOfYear - 187, 30) + 7);
                day = static_cast<int32>(floorMod(dayOfYear - 187, 30) + 1);
            }

            return { year, month, day };
        }

        /**
         * Обработка системного времени.
         */
        CalendarDate timeToCalendarDate(time_t unixTime) const noexcept override
        {
            // 1970-01-01 Григорианский = 719163 RD
            int64 fixedDay = 719163 + floorDiv(static_cast<int64>(unixTime), 86400);
            return fixedToDate(fixedDay);
        }

        /**
         * Определение дня недели.
         * RD 1 был Понедельником.
         */
        int32 getDayOfWeekIndex(int64 fixed) const noexcept override
        {
            // Используем floorMod, чтобы индекс всегда был положительным (0-6)
            // При RD 1: (1 + 6) % 7 = 0 (Monday)
            return static_cast<int32>(floorMod(fixed + 6, 7));
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