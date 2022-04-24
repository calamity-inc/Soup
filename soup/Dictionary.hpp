#pragma once

#include "fwd.hpp"

#include "DictionaryWord.hpp"
#include <map>

namespace soup
{
	SOUP_PACKET(Dictionary)
	{
		std::map<std::string, DictionaryWord> words{};

		void addWords(const JsonObject& obj); // Made for data from https://stackoverflow.com/a/54982015

		[[nodiscard]] const DictionaryWord* find(std::string word) const;
		[[nodiscard]] const DictionaryWord& at(std::string word) const;

		SOUP_PACKET_IO(s)
		{
			u8 format_version = 0;
			if (!s.u8(format_version)
				|| format_version != 0
				)
			{
				return false;
			}
			if constexpr (s.isRead())
			{
				while (s.hasMore())
				{
					std::string word{};
					DictionaryWord dw{};
					if (!s.str_nt(word)
						|| !dw.io(s)
						)
					{
						return false;
					}
					words.emplace(std::move(word), std::move(dw));
				}
			}
			else if constexpr (s.isWrite())
			{
				for (auto& e : words)
				{
					if (!s.str_nt(e.first)
						|| !e.second.io(s))
					{
						return false;
					}
				}
			}
			return true;
		}
	};
}
