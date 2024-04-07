#pragma once

#include "Packet.hpp"

#include <map>
#include <string>
#include <vector>

#include "StringPool.hpp"

NAMESPACE_SOUP
{
	struct Joke
	{
		const char* type;
		std::string setup;
		std::string punchline;

		[[nodiscard]] bool isSingleLine() const;
		[[nodiscard]] std::string toString() const;
	};

	SOUP_PACKET(Jokebook)
	{
		std::vector<Joke> jokes;
		StringPool type_pool;

		[[nodiscard]] const Joke& getRandomJoke() const;

		[[nodiscard]] bool hasType(const std::string& type) const;
		[[nodiscard]] const Joke& getRandomJokeByType(const std::string& type) const;
		[[nodiscard]] std::map<const char*, size_t> getNumJokesByType() const;

		void addJoke(const std::string& type, std::string setup, std::string punchline);

		void loadJson(const JsonArray& arr);
		// Made for data from https://raw.githubusercontent.com/Sainan/official_joke_api/master/jokes/index.json
		// or https://raw.githubusercontent.com/15Dkatz/official_joke_api/master/jokes/index.json

		SOUP_PACKET_IO(s)
		{
			uint64_t size;
			SOUP_IF_ISREAD
			{
				if (!s.u64_dyn(size))
				{
					return false;
				}
				jokes.clear();
				jokes.reserve(static_cast<size_t>(size));
				type_pool.clear();
				while (size--)
				{
					std::string type, setup, punchline;
					if (!s.str_nt(type) || !s.str_nt(setup) || !s.str_nt(punchline))
					{
						return false;
					}
					addJoke(type, std::move(setup), std::move(punchline));
				}
			}
			SOUP_ELSEIF_ISWRITE
			{
				size = jokes.size();
				if (!s.u64_dyn(size))
				{
					return false;
				}
				for (const auto& j : jokes)
				{
					if (!s.str_nt(j.type)
						|| !s.str_nt(j.setup)
						|| !s.str_nt(j.punchline)
						)
					{
						return false;
					}
				}
			}
			return true;
		}
	};
}
