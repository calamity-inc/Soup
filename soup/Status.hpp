#pragma once

#include "macros.hpp"

#include <string>

#include "Capture.hpp"

#define SOUP_THROW(err) return ::soup::Status(err)
#define SOUP_RETHROW return ::soup::Status(::soup::Status::getLastError());
#define SOUP_CHECK(status) if (!(status)) { SOUP_RETHROW; }

#define SOUP_SUCCESS_0() return ::soup::Status();
#define SOUP_SUCCESS_1(res) return ::soup::Result<decltype(res)>(std::move(res));

#define SOUP_SUCCESS(...) MULTI_MACRO(SOUP_SUCCESS, __VA_ARGS__)

namespace soup
{
	struct Status
	{
		const bool success;
		Capture res{};

		inline static thread_local std::string last_err{};

		Status() noexcept
			: success(true)
		{
		}

		Status(Capture&& res)
			: success(true), res(std::move(res))
		{
		}

		Status(const char* err)
			: success(false)
		{
			last_err = err;
		}
		
		Status(std::string err)
			: success(false)
		{
			last_err = std::move(err);
		}

		Status(Status&& b)
			: success(b.success), res(std::move(b.res))
		{
		}

		[[nodiscard]] operator bool() const noexcept
		{
			return success;
		}

		[[nodiscard]] static const std::string& getLastError() noexcept
		{
			return last_err;
		}
	};

	template <typename T>
	struct Result : public Status
	{
		Result(T&& val)
			: Status(Capture(std::move(val)))
		{
		}

		Result(Status&& b)
			: Status(std::move(b))
		{
		}

		[[nodiscard]] T* operator ->() const noexcept
		{
			return res.get<T*>();
		}
	};
}
