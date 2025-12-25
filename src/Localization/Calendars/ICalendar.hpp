#ifndef NB_SRC_LOCALIZATION_CALENDARS_ICALENDAR_HPP
#define NB_SRC_LOCALIZATION_CALENDARS_ICALENDAR_HPP
#include <Types.hpp>
#include <string_view>
#include <string>
#include "CalendarDate.hpp"


namespace Localization
{
    class ICalendar
    {
    public:
        virtual ~ICalendar() noexcept = default;

        virtual int64 dateToFixed(int32 y, int32 m, int32 d) const noexcept = 0;
        virtual CalendarDate fixedToDate(int64 fixed) const noexcept = 0;
        virtual CalendarDate timeToCalendarDate(time_t unixTime) const noexcept = 0;

        virtual int32 getDaysInMonth(int32 year, int32 month) const noexcept = 0;
        virtual int32 getMonthsInYear(int32 year) const noexcept = 0;
        virtual bool isLeapYear(int32 year) const noexcept = 0;

        virtual int32 getDaysInWeek() const noexcept = 0;

        virtual int32 getDayOfWeekIndex(int64 fixed) const noexcept = 0;

        virtual std::string_view getSystemName() const noexcept = 0;
        virtual std::string_view getMonthKey(int32 year, int32 month) const noexcept = 0;
        virtual std::string getDayOfWeekKey(int32 dowIndex) const noexcept = 0;
    };
};
#endif 
