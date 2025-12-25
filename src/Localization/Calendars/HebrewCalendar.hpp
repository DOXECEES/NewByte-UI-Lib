#ifndef NB_SRC_LOCALIZATION_CALENDARS_HEBREWCALENDAR_HPP
#define NB_SRC_LOCALIZATION_CALENDARS_HEBREWCALENDAR_HPP

#include "ICalendar.hpp"
#include "Localization/Translation.hpp"
#include <array>
#include <string_view>
#include <ctime>

namespace Localization
{
    class HebrewCalendar final : public ICalendar
    {
    public:
        static constexpr std::string_view SYSTEM_NAME = "hebrew";
        static constexpr int32 DAYS_IN_WEEK = 7;

        // Эпоха: 1 Тишрея 1 года (7 октября 3761 г. до н.э. по Григорианскому)
        static constexpr int64 HEBREW_EPOCH = -1373427;

        // Названия месяцев (ключи для локализации)
        static constexpr std::array<std::string_view, 14> MONTHS_KEYS = {
            "Calendar.Hebrew.Month_Tishrei",   // 1
            "Calendar.Hebrew.Month_Cheshvan",  // 2
            "Calendar.Hebrew.Month_Kislev",    // 3
            "Calendar.Hebrew.Month_Tevet",     // 4
            "Calendar.Hebrew.Month_Shevat",    // 5
            "Calendar.Hebrew.Month_Adar",      // 6 (в обычном году)
            "Calendar.Hebrew.Month_Adar_I",    // 7 (только в високосном)
            "Calendar.Hebrew.Month_Adar_II",   // 8 (только в високосном)
            "Calendar.Hebrew.Month_Nisan",     // 9
            "Calendar.Hebrew.Month_Iyar",      // 10
            "Calendar.Hebrew.Month_Sivan",     // 11
            "Calendar.Hebrew.Month_Tammuz",    // 12
            "Calendar.Hebrew.Month_Av",        // 13
            "Calendar.Hebrew.Month_Elul"       // 14
        };

        std::string_view getSystemName() const noexcept override { return SYSTEM_NAME; }
        int32 getDaysInWeek() const noexcept override { return DAYS_IN_WEEK; }

        bool isLeapYear(int32 year) const noexcept override {
            // 19-летний цикл (Метонов цикл): 3, 6, 8, 11, 14, 17, 19 годы - високосные
            return ((7 * static_cast<int64>(year) + 1) % 19) < 7;
        }

        int32 getMonthsInYear(int32 year) const noexcept override {
            return isLeapYear(year) ? 13 : 12;
        }

        // --- Математическое ядро ---

        // Проверка переноса нового года (Dechiyot)
        int64 elapsedDays(int32 year) const noexcept {
            int64 months = (235LL * year - 234) / 19;
            int64 parts = 12084LL + 13753LL * months;
            int64 day = 29LL * months + parts / 25920;
            parts %= 25920;

            // Правило 1: Rosh Hashanah не может быть в Вс, Ср, Пт
            int64 dayOfWeek = day % 7;
            if (dayOfWeek == 2 || dayOfWeek == 4 || dayOfWeek == 6) {
                day++;
            }
            else {
                if (parts >= 19440) {
                    day++;
                    return elapsedDaysCorrection(day);
                }
                if (!isLeapYear(year) && dayOfWeek == 1 && parts >= 9924) {
                    day += 2;
                }
                else if (isLeapYear(year - 1) && dayOfWeek == 0 && parts >= 15895) {
                    day++;
                }
            }
            return elapsedDaysCorrection(day);
        }

        int64 elapsedDaysCorrection(int64 day) const noexcept {
            int64 dw = day % 7;
            if (dw == 2 || dw == 4 || dw == 6) return day + 1;
            return day;
        }

        int32 getDaysInYear(int32 year) const noexcept {
            return static_cast<int32>(elapsedDays(year + 1) - elapsedDays(year));
        }

        int32 getDaysInMonth(int32 year, int32 month) const noexcept override {
            // 1. Стандартные месяцы
            if (month == 1 || month == 5 || month == 8 || month == 10 || month == 12) return 30; // Tishrei, Shevat, Nisan, Sivan, Av
            if (month == 4 || month == 7 || month == 9 || month == 11 || month == 13) return 29; // Tevet, Adar II, Iyar, Tammuz, Elul

            // 2. Месяцы с переменной длиной (зависят от длины года)
            int32 yearDays = getDaysInYear(year);
            bool isLongYear = (yearDays % 10 == 5);
            bool isShortYear = (yearDays % 10 == 3);

            if (month == 2) return isLongYear ? 30 : 29; // Cheshvan
            if (month == 3) return isShortYear ? 29 : 30; // Kislev

            // 3. Адар (6-й и 7-й месяцы)
            if (month == 6) return isLeapYear(year) ? 30 : 29; // Adar I (30) или Adar (29)

            return 29;
        }

        int64 dateToFixed(int32 year, int32 month, int32 day) const noexcept override {
            int64 fixed = HEBREW_EPOCH + elapsedDays(year) - 1 + day;
            for (int32 m = 1; m < month; ++m) {
                fixed += getDaysInMonth(year, m);
            }
            return fixed;
        }

        CalendarDate fixedToDate(int64 fixed) const noexcept override {
            int32 year = static_cast<int32>((fixed - HEBREW_EPOCH) / 365) + 1;
            while (dateToFixed(year + 1, 1, 1) <= fixed) year++;
            while (dateToFixed(year, 1, 1) > fixed) year--;

            int32 month = 1;
            int64 day = fixed - dateToFixed(year, 1, 1) + 1;
            while (month < getMonthsInYear(year)) {
                int32 dim = getDaysInMonth(year, month);
                if (day <= dim) break;
                day -= dim;
                month++;
            }
            return { year, month, static_cast<int>(day) };
        }

        // --- Интернационализация ---

        std::string_view getMonthKey(int32 year, int32 month) const noexcept override {
            if (month < 1 || month > 13) return {};

            bool leap = isLeapYear(year);
            // Индексы ключей:
            // В обычном году: 1-5 (Tish-Shev), 6 (Adar), 9-14 (Nis-Elul)
            // В високосном: 1-5, 7 (Adar I), 8 (Adar II), 9-14

            if (!leap) {
                if (month < 6) return Translation::fromKey(MONTHS_KEYS[month - 1].data());
                if (month == 6) return Translation::fromKey(MONTHS_KEYS[5].data()); // Adar
                return Translation::fromKey(MONTHS_KEYS[month].data()); // Сдвиг на 1, пропуская Adar I/II
            }
            else {
                if (month < 6) return Translation::fromKey(MONTHS_KEYS[month - 1].data());
                if (month == 6) return Translation::fromKey(MONTHS_KEYS[6].data()); // Adar I
                if (month == 7) return Translation::fromKey(MONTHS_KEYS[7].data()); // Adar II
                return Translation::fromKey(MONTHS_KEYS[month].data());
            }
        }

        std::string getDayOfWeekKey(int32 index) const noexcept override {
            static constexpr std::array<std::string_view, 7> WEEK_KEYS = {
                "Calendar.Gregorian.Weekday_01_Full", "Calendar.Gregorian.Weekday_02_Full",
                "Calendar.Gregorian.Weekday_03_Full", "Calendar.Gregorian.Weekday_04_Full",
                "Calendar.Gregorian.Weekday_05_Full", "Calendar.Gregorian.Weekday_06_Full",
                "Calendar.Gregorian.Weekday_07_Full"
            };
            return (index >= 0 && index < 7) ? Translation::fromKey(WEEK_KEYS[index].data()) : "ddd";
        }

        CalendarDate timeToCalendarDate(time_t unixTime) const noexcept override {
            int64 fixedDay = 719163 + (static_cast<int64>(unixTime) / 86400);
            return fixedToDate(fixedDay);
        }

        int32 getDayOfWeekIndex(int64 fixed) const noexcept override {
            return static_cast<int32>((fixed + 6) % 7);
        }
    };
}

#endif