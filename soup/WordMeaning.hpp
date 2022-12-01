#pragma once

#include "Packet.hpp"
#include <string>
#include <vector>
#include "WordType.hpp"

namespace soup
{
	SOUP_PACKET(WordMeaning)
	{
		WordType type;
		std::string meaning;
		std::vector<std::string> context;
		std::vector<std::string> example;

		SOUP_PACKET_IO(s)
		{
			SOUP_IF_ISREAD
			{
				uint8_t this_is_why_you_dont_use_enums;
				if (!s.u8(this_is_why_you_dont_use_enums))
				{
					return false;
				}
				type = (WordType)this_is_why_you_dont_use_enums;
			}
			SOUP_ELSEIF_ISWRITE
			{
				uint8_t this_is_why_you_dont_use_enums = type;
				if (!s.u8(this_is_why_you_dont_use_enums))
				{
					return false;
				}
			}
			return s.str_nt(meaning)
				&& s.vec_str_nt_u64_dyn(context)
				&& s.vec_str_nt_u64_dyn(example)
				;
		}
	};
}
