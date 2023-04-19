#pragma once

#include <string>

namespace soup
{
	struct StringParser
	{
		std::string data;
		std::string::const_iterator it;

		StringParser(std::string data)
			: data(std::move(data)), it(this->data.cbegin())
		{
		}

		[[nodiscard]] size_t getPosition() const noexcept
		{
			return getPosition(it);
		}

		[[nodiscard]] size_t getPosition(std::string::const_iterator it) const noexcept
		{
			return it - data.cbegin();
		}
	};
}
