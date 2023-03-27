#include "Dictionary.hpp"

#include "JsonArray.hpp"
#include "JsonObject.hpp"
#include "JsonString.hpp"
#include "string.hpp"

namespace soup
{
	[[nodiscard]] static std::vector<std::string> processRelatedWords(const std::string& word, const JsonArray& arr)
	{
		std::vector<std::string> res{};
		for (const auto& e : arr.children)
		{
			std::string rel_word = e->asStr();
			if (rel_word.empty())
			{
				continue;
			}
			string::lower(rel_word);
			if (rel_word != word)
			{
				res.emplace_back(rel_word);
			}
		}
		return res;
	}

	void Dictionary::addWords(const JsonObject& obj)
	{
		for (const auto& e : obj.children)
		{
			std::string word = e.first->asStr();
			string::lower(word);

			JsonObject& data = e.second->asObj();

			DictionaryWord dw{};
			dw.antonyms = processRelatedWords(word, data.at("ANTONYMS").asArr());
			dw.synonyms = processRelatedWords(word, data.at("SYNONYMS").asArr());
			for (const auto& e2 : data.at("MEANINGS").asObj())
			{
				JsonArray& mdata = e2.second->asArr();
				WordMeaning m{};
				std::string type_str = mdata.at(0).asStr();
				if (type_str == "Noun")
				{
					m.type = NOUN;
				}
				else if (type_str == "Adjective")
				{
					m.type = ADJECTIVE;
				}
				else if (type_str == "Verb")
				{
					m.type = VERB;
				}
				else //if (type_str == "Adverb")
				{
					m.type = ADVERB;
				}
				m.meaning = mdata.at(1).asStr();
				for (const auto& ctx : mdata.at(2).asArr())
				{
					m.context.emplace_back(ctx.asStr());
				}
				for (const auto& ex : mdata.at(3).asArr())
				{
					m.example.emplace_back(ex.asStr());
				}
				dw.meanings.emplace_back(std::move(m));
			}

			words.emplace(std::move(word), std::move(dw));
		}
	}

	const DictionaryWord* Dictionary::find(std::string word) const
	{
		string::lower(word);
		auto e = words.find(word);
		if (e == words.end())
		{
			return nullptr;
		}
		return &e->second;
	}

	const DictionaryWord& Dictionary::at(std::string word) const
	{
		string::lower(word);
		return words.at(word);
	}
}
