#pragma once

#include <vector>

#include "rflMember.hpp"

namespace soup
{
	struct rflStruct
	{
		std::string name;
		std::vector<rflMember> members;

		[[nodiscard]] std::string toString() const noexcept
		{
			std::string str = "struct ";
			str.append(name);
			str.append("\n{\n");
			for (const auto& member : members)
			{
				str.push_back('\t');
				str.append(member.toString());
				str.append(";\n");
			}
			str.append("};\n");
			return str;
		}
	};
}
