#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct netAs
	{
		uint32_t number;
		std::string handle;
		std::string name;

		netAs() = default;

		netAs(uint32_t number, std::string&& name)
			: number(number), handle(name), name(std::move(name))
		{
		}
	};
}
