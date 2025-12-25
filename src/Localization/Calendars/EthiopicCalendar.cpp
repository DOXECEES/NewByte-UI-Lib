#include "EthiopicCalendar.hpp"

namespace Localization
{
	std::string_view EthiopicCalendar::getSystemName() const noexcept
	{
		return SYSTEM_NAME;
	}

	int32 EthiopicCalendar::getDaysInWeek() const noexcept
	{
		return DAYS_IN_WEEK;
	}

	int32 EthiopicCalendar::getMonthsInYear(int32 year) const noexcept
	{
		return MONTHS_IN_YEAR;
	}

	bool EthiopicCalendar::isLeapYear(int32 year) const noexcept
	{
		return (year % LEAP_CYCLE_YEARS) == 3;
	}

	int32 EthiopicCalendar::getDaysInMonth(int32 year, int32 month) const noexcept
	{
		if (month < 1 || month > MONTHS_IN_YEAR)
		{
			return 0;
		}

		if (month != PAGUME_MONTH)
		{
			return DAYS_IN_MONTH;
		}

		return isLeapYear(year) ? PAGUME_DAYS_LEAP : PAGUME_DAYS_COMMON;
	}

	int64 EthiopicCalendar::dateToFixed(int32 year, int32 month, int32 day) const noexcept
	{
		const int64 y = static_cast<int64>(year);

		return ETHIOPIC_EPOCH_FIXED
			+ (y - 1) * DAYS_IN_YEAR
			+ (y / LEAP_CYCLE_YEARS)
			+ (month - 1) * DAYS_IN_MONTH
			+ (day - 1);
	}

	CalendarDate EthiopicCalendar::fixedToDate(int64 fixed) const noexcept
	{
		const int32 year = static_cast<int32>(
			(LEAP_CYCLE_YEARS * (fixed - ETHIOPIC_EPOCH_FIXED) + (LEAP_CYCLE_DAYS - 1))
			/ LEAP_CYCLE_DAYS
		);

		const int64 startOfYear = dateToFixed(year, 1, 1);
		const int32 dayOfYear = static_cast<int32>(fixed - startOfYear);

		const int32 month = (dayOfYear / DAYS_IN_MONTH) + 1;
		const int32 day = (dayOfYear % DAYS_IN_MONTH) + 1;

		return { year, month, day };
	}

	CalendarDate EthiopicCalendar::timeToCalendarDate(time_t unixTime) const noexcept
	{
		const int64 fixed = UNIX_EPOCH_FIXED + (static_cast<int64>(unixTime) / SECONDS_PER_DAY);

		return fixedToDate(fixed);
	}

	int32 EthiopicCalendar::getDayOfWeekIndex(int64 fixed) const noexcept
	{
		return static_cast<int32>((fixed + DAYS_IN_WEEK - 1) % DAYS_IN_WEEK);
	}

	std::string_view EthiopicCalendar::getMonthKey(int32 year, int32 month) const noexcept
	{
		if (month < 1 || month > MONTHS_IN_YEAR)
		{
			return {};
		}

		return Translation::fromKey(MONTHS_KEYS[month - 1].data());
	}

	std::string EthiopicCalendar::getDayOfWeekKey(int32 index) const noexcept
	{
		static constexpr nbstl::Array<std::string_view, DAYS_IN_WEEK> WEEK_KEYS = {
			"Calendar.Gregorian.Weekday_01_Full",
			"Calendar.Gregorian.Weekday_02_Full",
			"Calendar.Gregorian.Weekday_03_Full",
			"Calendar.Gregorian.Weekday_04_Full",
			"Calendar.Gregorian.Weekday_05_Full",
			"Calendar.Gregorian.Weekday_06_Full",
			"Calendar.Gregorian.Weekday_07_Full"
		};

		if (index < 0 || index >= DAYS_IN_WEEK)
		{
			return "";
		}

		return Translation::fromKey(WEEK_KEYS[index].data());
	}


};