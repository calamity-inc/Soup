#pragma once

#include <string>

namespace soup
{
	struct StringParser
	{
		std::string data;
		const char* it;

		StringParser(std::string data)
			: data(std::move(data)), it(this->data.data())
		{
		}

		[[nodiscard]] size_t getPosition() const noexcept
		{
			return getPosition(it);
		}

		[[nodiscard]] size_t getPosition(const char* it) const noexcept
		{
			return it - data.data();
		}
	};
}
