#pragma once

#include <unordered_set>

namespace soup
{
	struct StringPool
	{
		std::unordered_set<std::string> pool;

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

		[[nodiscard]] bool contains(const std::string& str) const
		{
			return pool.find(str) != pool.end();
		}

		void clear() noexcept
		{
			pool.clear();
		}
	};
}
