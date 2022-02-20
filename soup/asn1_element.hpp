#pragma once

#include "asn1_identifier.hpp"

#include <string>

namespace soup
{
	struct asn1_element
	{
		asn1_identifier identifier;
		std::string data;
	};
}
