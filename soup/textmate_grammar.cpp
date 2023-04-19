#include "textmate_grammar.hpp"

#include "plist.hpp"

namespace soup
{
	void TmGrammar::fromPlist(const PlistDict& dict)
	{
		for (const auto& e : dict.children.at("patterns")->asArray().children)
		{
			patterns.emplace_back(patternFromPlist(e->asDict()));
		}
		if (auto pr = dict.children.find("repository"); pr != dict.children.end())
		{
			for (const auto& e : pr->second->asDict().children)
			{
				for (const auto& p : e.second->asDict().children.at("patterns")->asArray().children)
				{
					std::vector<TmPattern> patterns{};
					patterns.emplace_back(patternFromPlist(p->asDict()));
					repository.emplace(e.first, std::move(patterns));
				}
			}
		}
	}

	TmPattern TmGrammar::patternFromPlist(const PlistDict& dict)
	{
		std::string name{};
		if (auto e = dict.children.find("name"); e != dict.children.end())
		{
			name = e->second->asString().data;
		}
		if (auto e = dict.children.find("match"); e != dict.children.end())
		{
			return TmPattern(std::move(name), std::regex(e->second->asString().data));
		}
		throw 0;
	}

	const TmPattern* TmParser::getNextMatch(const std::vector<TmPattern>& patterns) const
	{
		const TmPattern* best = nullptr;
		for (const auto& pattern : patterns)
		{
			if (std::regex_search(it, data.cend(), pattern.res, pattern.match))
			{
				auto offset = pattern.res.position();
				if (offset == 0)
				{
					return &pattern;
				}
				if (best == nullptr
					|| best->res.position() > offset
					)
				{
					best = &pattern;
				}
			}
		}
		return best;
	}

	void TmParser::processPatternList(const std::vector<TmPattern>& patterns, std::vector<TmClassifiedText>& result)
	{
		while (true)
		{
			const TmPattern* pattern = getNextMatch(patterns);
			if (!pattern)
			{
				break;
			}

			std::vector<TmClassifiedText> captures{};

			for (size_t i = 0; i != pattern->res.size(); ++i)
			{
				if (i >= pattern->captures.size())
				{
					break;
				}
				if (!pattern->captures.at(i).empty())
				{
					captures.emplace_back(TmClassifiedText(getPosition(it + pattern->res.position(i)), (size_t)pattern->res.length(i), pattern->captures.at(i)));
				}
			}

			it += pattern->res.position();
			size_t start_pos = getPosition();
			it += pattern->res.length();

			if (pattern->is_end)
			{
				break;
			}

			if (pattern->end_match.has_value())
			{
				TmClassifiedText list(start_pos, 0, pattern->name, std::move(captures));

				std::vector<TmPattern> subpatterns = pattern->patterns;
				subpatterns.emplace_back(TmPattern({}, pattern->end_match.value(), pattern->end_captures)).is_end = true;
				processPatternList(subpatterns, list.children);

				list.len = getPosition() - start_pos;

				result.emplace_back(std::move(list));
			}
			else
			{
				result.emplace_back(TmClassifiedText(start_pos, getPosition() - start_pos, pattern->name, std::move(captures)));
			}
		}
	}
}
