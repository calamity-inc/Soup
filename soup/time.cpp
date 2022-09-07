#include "time.hpp"

#include "base.hpp"

#if SOUP_WINDOWS
#define timegm _mkgmtime
#endif

namespace soup
{
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

		struct tm timeinfo;
		timeinfo.tm_year = year - 1900;
		timeinfo.tm_mon = month - 1;
		timeinfo.tm_mday = day;
		timeinfo.tm_hour = hour;
		timeinfo.tm_min = minute;
		timeinfo.tm_sec = second;
		return timegm(&timeinfo);

	}
}
