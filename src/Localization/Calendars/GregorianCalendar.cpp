#include "GregorianCalendar.hpp"

namespace Localization
{
	std::string_view GregorianCalendar::getSystemName() const noexcept
	{
		return SYSTEM_NAME;
	}

	int32 GregorianCalendar::getDaysInWeek() const noexcept
	{
		return DAYS_IN_WEEK;
	}

	int32 GregorianCalendar::getMonthsInYear(int32 year) const noexcept
	{
		return MONTHS_IN_YEAR;
	}

	bool GregorianCalendar::isLeapYear(int32 year) const noexcept
	{
		return (year % YEARS_IN_4_CYCLE == 0)
			&& ((year % YEARS_IN_100_CYCLE != 0)
				|| (year % YEARS_IN_400_CYCLE == 0));
	}

	int32 GregorianCalendar::getDaysInMonth(int32 year, int32 month) const noexcept
	{
		if (month < 1 || month > MONTHS_IN_YEAR)
		{
			return 0;
		}

		if (month == 2 && isLeapYear(year))
		{
			return 29;
		}

		return DAYS_PER_MONTH[month - 1];
	}

	int64 GregorianCalendar::dateToFixed(int32 year, int32 month, int32 day) const noexcept
	{
		const int64 y = static_cast<int64>(year) - 1;

		int64 fixed =
			day +
			DAYS_IN_YEAR * y +
			y / YEARS_IN_4_CYCLE -
			y / YEARS_IN_100_CYCLE +
			y / YEARS_IN_400_CYCLE;

		fixed += (367 * month - 362) / 12;

		if (month > 2)
		{
			fixed += isLeapYear(year) ? -1 : -2;
		}

		return fixed;
	}

	CalendarDate GregorianCalendar::fixedToDate(int64 fixed) const noexcept
	{
		const int64 d0 = fixed - 1;

		const int64 n400 = d0 / DAYS_IN_400_CYCLE;
		const int64 d1 = d0 % DAYS_IN_400_CYCLE;

		const int64 n100 = d1 / DAYS_IN_100_CYCLE;
		const int64 d2 = d1 % DAYS_IN_100_CYCLE;

		const int64 n4 = d2 / DAYS_IN_4_CYCLE;
		const int64 d3 = d2 % DAYS_IN_4_CYCLE;

		const int64 n1 = d3 / DAYS_IN_YEAR;

		int32 year = static_cast<int32>(
			YEARS_IN_400_CYCLE * n400 +
			YEARS_IN_100_CYCLE * n100 +
			YEARS_IN_4_CYCLE * n4 +
			n1
			);

		if (n100 != 4 && n1 != 4)
		{
			++year;
		}

		int64 dayOfYear = fixed - dateToFixed(year, 1, 1) + 1;

		int32 month = 1;
		for (; month <= MONTHS_IN_YEAR; ++month)
		{
			const int32 dim = getDaysInMonth(year, month);
			if (dayOfYear <= dim)
			{
				break;
			}
			dayOfYear -= dim;
		}

		return { year, month, static_cast<int32>(dayOfYear) };
	}

	CalendarDate GregorianCalendar::timeToCalendarDate(time_t unixTime) const noexcept
	{
		const int64 fixed = UNIX_EPOCH_FIXED + (static_cast<int64>(unixTime) / SECONDS_PER_DAY);

		return fixedToDate(fixed);
	}

	int32 GregorianCalendar::getDayOfWeekIndex(int64 fixed) const noexcept
	{
		return static_cast<int32>((fixed + DAYS_IN_WEEK - 1) % DAYS_IN_WEEK);
	}

	std::string_view GregorianCalendar::getMonthKey(int32 year, int32 month) const noexcept
	{
		if (month < 1 || month > MONTHS_IN_YEAR)
		{
			return {};
		}

		return Translation::fromKey(MONTHS_KEYS[month - 1].data());
	}

	std::string GregorianCalendar::getDayOfWeekKey(int32 index) const noexcept
	{
		if (index < 0 || index >= DAYS_IN_WEEK)
		{
			return "";
		}

		return Translation::fromKey(WEEK_KEYS[index].data());
	}

};

