#pragma once

#include <chrono>
#include <ctime>
#include <string>

namespace soup
{
	struct Datetime
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;

		int wday; // days since sunday (0-6)

		[[nodiscard]] static Datetime fromTm(const struct tm& t);
	};

	struct time
	{
		// System-dependent time

		[[nodiscard]] static std::time_t millis() noexcept
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		}

		[[nodiscard]] static std::time_t millisSince(std::time_t since) noexcept
		{
			return millis() - since;
		}

		[[nodiscard]] static std::time_t nanos() noexcept
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		}

		// UNIX time (seconds since 00:00, Jan 1, 1970 UTC)

		[[nodiscard]] static std::time_t unixSeconds() noexcept
		{
			return ::time(nullptr); // <-- ~0.0000006ms
			//                           v ~0.000002ms
			//return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		}

		[[nodiscard]] static Datetime datetimeUtc(std::time_t ts) noexcept; // construct datetime based on UNIX time stamp
		[[nodiscard]] static Datetime datetimeLocal(std::time_t ts) noexcept; // construct datetime based on local time at UNIX time stamp

		[[nodiscard]] static int getLocalTimezoneOffset() noexcept;

		[[nodiscard]] static std::string toRfc2822(std::time_t ts); // construct date and time string as defined in RFC 2822 (ex: "Thu, 1 Feb 00:00:00 GMT") based on UNIX time stamp

		[[nodiscard]] static std::time_t toUnix(const Datetime& dt);
		[[nodiscard]] static std::time_t toUnix(int year, int month, int day, int hour, int minute, int second);
	};
}
