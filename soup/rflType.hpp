#pragma once

#include <cstdint>
#include <string>

NAMESPACE_SOUP
{
	struct rflType
	{
		enum AccessType : uint8_t
		{
			DIRECT = 0,
			POINTER,
			REFERENCE,
			RVALUE_REFERENCE,
		};

		std::string name;
		AccessType at;

		[[nodiscard]] std::string toString() const
		{
			std::string str = name;
			const char* access_type_strings[] = { "", "*", "&", "&&" };
			str.append(access_type_strings[at]);
			return str;
		}
	};
}
