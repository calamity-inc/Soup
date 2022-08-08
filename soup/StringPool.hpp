#pragma once

#include <unordered_set>

namespace soup
{
	class StringPool
	{
	private:
		std::unordered_set<std::string> pool;

	public:
		[[nodiscard]] const char* emplace(const std::string& str)
		{
			return pool.emplace(str).first->c_str();
		}

		[[nodiscard]] const char* emplace(std::string&& str)
		{
			return pool.emplace(std::move(str)).first->c_str();
		}

		[[nodiscard]] bool empty() const noexcept
		{
			return pool.empty();
		}

		void clear() noexcept
		{
			pool.clear();
		}
	};
}
