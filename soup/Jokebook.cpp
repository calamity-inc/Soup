#include "Jokebook.hpp"

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonString.hpp"
#include "rand.hpp"

namespace soup
{
	bool Joke::isSingleLine() const
	{
		return setup.find('\n') == std::string::npos
			&& setup.find(": ") == std::string::npos
			;
	}
	std::string Joke::toString() const
	{
		std::string str = setup;
		str.push_back(isSingleLine() ? ' ' : '\n');
		str.append(punchline);
		return str;
	}

	const Joke& Jokebook::getRandomJoke() const
	{
		return soup::rand(jokes);
	}

	bool Jokebook::hasType(const std::string& type) const
	{
		return type_pool.contains(type);
	}

	const Joke& Jokebook::getRandomJokeByType(const std::string& type) const
	{
		if (!hasType(type))
		{
			throw 0;
		}
		while (true)
		{
			const Joke& j = getRandomJoke();
			if (j.type == type)
			{
				return j;
			}
		}
	}

	std::map<const char*, size_t> Jokebook::getNumJokesByType() const
	{
		std::map<const char*, size_t> map{};
		for (const auto& j : jokes)
		{
			auto e = map.find(j.type);
			if (e != map.end())
			{
				++e->second;
			}
			else
			{
				map.emplace(j.type, 1);
			}
		}
		return map;
	}

	void Jokebook::addJoke(const std::string& type, std::string setup, std::string punchline)
	{
		jokes.emplace_back(Joke{ type_pool.emplace(type), std::move(setup), std::move(punchline) });
	}

	void Jokebook::loadJson(const JsonArray& arr)
	{
		jokes.reserve(jokes.size() + arr.size());
		for (const auto& elm : arr)
		{
			addJoke(
				elm.asObj().at("type").asStr().value,
				elm.asObj().at("setup").asStr().value,
				elm.asObj().at("punchline").asStr().value
			);
		}
	}
}
