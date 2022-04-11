#pragma once

#include "word_meaning.hpp"

namespace soup
{
	SOUP_PACKET(dictionary_word)
	{
		std::vector<word_meaning> meanings{};
		std::vector<std::string> antonyms{};
		std::vector<std::string> synonyms{};

		SOUP_PACKET_IO(s)
		{
			if (s.isRead())
			{
				u64 num_meanings;
				if (!s.u64_dyn(num_meanings))
				{
					return false;
				}
				meanings.reserve(num_meanings);
				for (; num_meanings != 0; --num_meanings)
				{
					word_meaning meaning{};
					if (!meaning.io(s))
					{
						return false;
					}
					meanings.emplace_back(std::move(meaning));
				}
			}
			else if (s.isWrite())
			{
				auto num_meanings = meanings.size();
				if (!s.u64_dyn(num_meanings))
				{
					return false;
				}
				for (auto& e : meanings)
				{
					if (!e.io(s))
					{
						return false;
					}
				}
			}
			return s.vec_str_nt_u64_dyn(antonyms)
				&& s.vec_str_nt_u64_dyn(synonyms)
				;
		}
	};
}
