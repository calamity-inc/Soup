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
			return TmPattern(std::move(name), Regex(e->second->asString().data));
		}
		SOUP_ASSERT_UNREACHABLE;
	}

	const TmPattern* TmParser::getNextMatch(const std::vector<TmPattern>& patterns) const
	{
		const TmPattern* best = nullptr;
		for (const auto& pattern : patterns)
		{
			pattern.res = pattern.match.search(it, data.cend());
			if (pattern.res.isSuccess())
			{
				pattern.res_offset = std::distance(it, pattern.res.groups.at(0)->begin);
				if (pattern.res_offset == 0)
				{
					return &pattern;
				}
				if (best == nullptr
					|| best->res_offset > pattern.res_offset
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

			for (size_t i = 0; i != pattern->res.groups.size(); ++i)
			{
				if (i >= pattern->captures.size())
				{
					break;
				}
				if (!pattern->captures.at(i).empty())
				{
					captures.emplace_back(TmClassifiedText(getPosition(pattern->res.groups.at(i)->begin), (size_t)pattern->res.groups.at(i)->length(), pattern->captures.at(i)));
				}
			}

			it += pattern->res_offset;
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
