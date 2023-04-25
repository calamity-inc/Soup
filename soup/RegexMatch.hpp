#pragma once

#include <optional>
#include <vector>

#include "RegexMatchedGroup.hpp"

namespace soup
{
	struct RegexMatch
	{
		std::vector<std::optional<RegexMatchedGroup>> groups{};

		[[nodiscard]] std::string toString() const noexcept
		{
			std::string str{};
			for (size_t i = 0; i != groups.size(); ++i)
			{
				if (groups.at(i).has_value())
				{
					str.append(std::to_string(i));
					str.append("=\"");
					str.append(groups.at(i)->toString());
					str.append("\", ");
				}
			}
			if (!str.empty())
			{
				str.erase(str.size() - 2, 2);
			}
			return str;
		}
	};
}
