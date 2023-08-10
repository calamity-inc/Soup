#include "time.hpp"

#include "base.hpp"
#include "string.hpp"

#if SOUP_WINDOWS
#define timegm _mkgmtime
#endif

namespace soup
{
	static const char wdays[(7 * 3) + 1] = "SunMonTueWedThuFriSat";
	static const char months[(12 * 3) + 1] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	Datetime Datetime::fromTm(const struct tm& t)
	{
		Datetime dt;

		dt.year = t.tm_year + 1900;
		dt.month = t.tm_mon + 1;
		dt.day = t.tm_mday;
		dt.hour = t.tm_hour;
		dt.minute = t.tm_min;
		dt.second = t.tm_sec;

		dt.wday = t.tm_wday;

		return dt;
	}

	std::string Datetime::toString() const
	{
		std::string str;
		str.append(string::lpad(std::to_string(this->hour), 2, '0'));
		str.push_back(':');
		str.append(string::lpad(std::to_string(this->minute), 2, '0'));
		str.push_back(':');
		str.append(string::lpad(std::to_string(this->second), 2, '0'));
		str.append(", ");
		str.append(std::to_string(this->day));
		if (this->month != 0)
		{
			str.push_back(' ');
			str.append(&months[(this->month - 1) * 3], 3);
		}
		return str;
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

	std::string time::toRfc2822(std::time_t ts)
	{
		const auto dt = datetimeUtc(ts);

		std::string str(&wdays[dt.wday * 3], 3);
		str.append(", ");
		str.append(std::to_string(dt.day));
		str.push_back(' ');
		str.append(&months[(dt.month - 1) * 3], 3);
		str.push_back(' ');
		str.append(std::to_string(dt.year));
		str.push_back(' ');
		str.append(string::lpad(std::to_string(dt.hour), 2, '0'));
		str.push_back(':');
		str.append(string::lpad(std::to_string(dt.minute), 2, '0'));
		str.push_back(':');
		str.append(string::lpad(std::to_string(dt.second), 2, '0'));
		str.append(" GMT");
		return str;
	}

	std::string time::toIso8601(std::time_t ts)
	{
		const auto dt = datetimeUtc(ts);

		std::string str = std::to_string(dt.year);
		str.push_back('-');
		str.append(string::lpad(std::to_string(dt.month), 2, '0'));
		str.push_back('-');
		str.append(string::lpad(std::to_string(dt.day), 2, '0'));
		str.push_back('T');
		str.append(string::lpad(std::to_string(dt.hour), 2, '0'));
		str.push_back(':');
		str.append(string::lpad(std::to_string(dt.minute), 2, '0'));
		str.push_back(':');
		str.append(string::lpad(std::to_string(dt.second), 2, '0'));
		str.push_back('Z');
		return str;

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

	int time::isLeapYear(int year)
	{
		// We have a leap year if it is...
		return (year % 4) == 0 // divisible by 4 and either...
			&& (((year % 100) != 0) // not divisible by 100
				|| (year % 400) == 0 // or divisible by 400
				);
	}

	int time::getDaysInMonth(int year, int month)
	{
		switch (month)
		{
		case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
		case 2: return /*isLeapYear(year) ? 29 : 28*/ 28 + isLeapYear(year);
		case 4: case 6: case 9: case 11: return 30;
		}
		SOUP_ASSERT_UNREACHABLE;
	}
}
