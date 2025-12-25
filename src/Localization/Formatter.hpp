#ifndef NBUI_SRC_LOCALIZATION_FORMATTER_HPP
#define NBUI_SRC_LOCALIZATION_FORMATTER_HPP

#include "Localization/LocaleManager.hpp"
#include "Localization/Translation.hpp"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <Alghorithm.hpp>


namespace Localization
{
    class Formatter
    {
    public:

        static std::string toString(double value)
        {
            const Locale& loc = LocaleManager::current();

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << value;
            std::string s = oss.str();

            size_t dotPos = s.find('.');
            std::string intPart = (dotPos == std::string::npos) ? s : s.substr(0, dotPos);
            std::string fracPart = (dotPos == std::string::npos) ? "" : s.substr(dotPos + 1);

            std::string formattedInt;
            int count = 0;
            for (auto it = intPart.rbegin(); it != intPart.rend(); ++it)
            {
                if (count && count % 3 == 0)
                    formattedInt.insert(formattedInt.begin(), loc.number().thousandsSeparator);
                formattedInt.insert(formattedInt.begin(), *it);
                ++count;
            }

            if (!fracPart.empty())
                formattedInt += loc.number().decimalSeparator + fracPart;

            return formattedInt;
        }

        static std::string toCurrency(double value)
        {
            const Locale& loc = LocaleManager::current();
            return toString(value) + " " + loc.number().currencySymbol;
        }

        static std::string toPercent(double value)
        {
            const Locale& loc = LocaleManager::current();
            return toString(value) + loc.number().percentSymbol;
        }

        static std::string toDate(std::time_t time)
        {
            const Locale& loc = LocaleManager::current();
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &time);
#else
            localtime_r(&time, &tm);
#endif

            const std::string& format = loc.dateTime().date;

            auto pad = [](int value, size_t count)
            {
                std::string s = std::to_string(value);
                return (s.size() < count) ? std::string(count - s.size(), '0') + s : s;
            };

            auto getTranslated = [](const std::string& prefix, int value, bool full)
            {
                char buf[3];
                sprintf(buf, "%02d", value);
                std::string key = prefix + "_" + buf + (full ? "_Full" : "_Abbr");
                return Localization::Translation::fromKey(key);
            };

            const int year = tm.tm_year + 1900;
            const int month = tm.tm_mon + 1;
            const int day = tm.tm_mday;
            const int quarter = (month + 2) / 3;
            const int weekday = (tm.tm_wday == 0) ? 7 : tm.tm_wday;

            std::string out;
            out.reserve(format.size() + 32);

            for (size_t i = 0; i < format.size(); )
            {
                char symbol = format[i];
                size_t count = 0;
                while (i + count < format.size() && format[i + count] == symbol)
                    count++;

                switch (symbol)
                {
                case 'y': // --- √Œƒ ---
                    if (count == 2) out += pad(year % 100, 2);
                    else out += std::to_string(year);
                    break;

                case 'M': // --- Ã≈—ﬂ÷ ---
                    if (count >= 3) out += getTranslated("DateTime.Month", month, count == 4);
                    else            out += pad(month, count);
                    break;

                case 'd': // --- ƒ≈Õ‹ ---
                    out += pad(day, count);
                    break;

                case 'E': // --- ƒ≈Õ‹ Õ≈ƒ≈À» ---
                    out += getTranslated("DateTime.Weekday", weekday, count == 4);
                    break;

                case 'Q': // ---  ¬¿–“¿À ---
                    if (count >= 3) out += getTranslated("DateTime.Quarter", quarter, count == 4);
                    else            out += pad(quarter, count);
                    break;

                case 'w': // --- Õ≈ƒ≈Àﬂ √Œƒ¿ ---
                {
                    int week = (tm.tm_yday + 7 - (tm.tm_wday ? tm.tm_wday - 1 : 6)) / 7;
                    out += pad(week, count);
                    break;
                }

                default: // --- –¿«ƒ≈À»“≈À» (ÔÓ·ÂÎ˚, ÚÓ˜ÍË, Á‡ÔˇÚ˚Â) ---
                    out.append(count, symbol);
                    break;
                }

                i += count;
            }

            return out;
        }

        static std::string toTime(std::time_t time)
        {
            const Locale& loc = LocaleManager::current();
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &time);
#else
            localtime_r(&time, &tm);
#endif
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), loc.dateTime().time.c_str(), &tm);
            return std::string(buffer);
        }

        static std::string toDateTime(std::time_t time)
        {
            const Locale& loc = LocaleManager::current();
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &time);
#else
            localtime_r(&time, &tm);
#endif
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), loc.dateTime().dateTime.c_str(), &tm);
            return std::string(buffer);
        }

    private:
       

    };

}

#endif 
