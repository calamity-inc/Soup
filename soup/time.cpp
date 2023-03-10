#include "time.hpp"

#include "base.hpp"

#if SOUP_WINDOWS
#define timegm _mkgmtime
#endif

namespace soup
{
	Datetime Datetime::fromTm(const struct tm& t)
	{
		Datetime dt;
		dt.year = t.tm_year + 1900;
		dt.month = t.tm_mon + 1;
		dt.day = t.tm_mday;
		dt.hour = t.tm_hour;
		dt.minute = t.tm_min;
		dt.second = t.tm_sec;
		return dt;
	}

	Datetime time::datetimeUtc(std::time_t ts) noexcept
	{
#if SOUP_WINDOWS
		return Datetime::fromTm(*::gmtime(&ts));
#else
		struct tm t;
		::gmtime_r(&ts, &t);
		return Datetime::fromTm(t);
#endif
	}

	Datetime time::datetimeLocal(std::time_t ts) noexcept
	{
#if SOUP_WINDOWS
		return Datetime::fromTm(*::localtime(&ts));
#else
		struct tm t;
		::localtime_r(&ts, &t);
		return Datetime::fromTm(t);
#endif
	}

	int time::getLocalTimezoneOffset() noexcept
	{
		return datetimeLocal(0).hour - datetimeUtc(0).hour;
	}

	std::time_t time::toUnix(const Datetime& dt)
	{
		return toUnix(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
	}

	std::time_t time::toUnix(int year, int month, int day, int hour, int minute, int second)
	{
		/*time_t t = (year - 1970);
		t *= 12; t += (month - 1);
		t *= 30.44; t += (day - 1);
		t *= 24; t += hour;
		t *= 60; t += minute;
		t *= 60; t += second;
		return t;*/

		struct tm t;
		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		t.tm_hour = hour;
		t.tm_min = minute;
		t.tm_sec = second;
		return timegm(&t);

	}
}
