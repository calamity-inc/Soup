#pragma once

//#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "fwd.hpp"
#include "Regex.hpp"
#include "StringParser.hpp"

namespace soup
{
	struct TmPattern
	{
		std::string name;
		Regex match;
		std::vector<std::string> captures{};
		std::optional<Regex> end_match;
		std::vector<std::string> end_captures{};
		std::vector<TmPattern> patterns;

		mutable RegexMatchResult res;
		mutable size_t res_offset;
		bool is_end = false;

		TmPattern(std::string name, Regex match, std::vector<std::string> captures = {})
			: name(std::move(name)), match(std::move(match)), captures(std::move(captures))
		{
		}

		TmPattern(std::string name, Regex begin_match, Regex end_match, std::vector<TmPattern> patterns = {})
			: name(std::move(name)), match(std::move(begin_match)), end_match(std::move(end_match)), patterns(std::move(patterns))
		{
		}
	};

	struct TmGrammar
	{
		std::vector<TmPattern> patterns{};
		std::unordered_map<std::string, std::vector<TmPattern>> repository{};

		// fromPlist(plist::parse(...).at(0)->asDict())
		void fromPlist(const PlistDict& dict);

		[[nodiscard]] static TmPattern patternFromPlist(const PlistDict& dict);
	};

	struct TmClassifiedText
	{
		size_t pos;
		size_t len;
		std::string name;
		std::vector<TmClassifiedText> children;

		TmClassifiedText(size_t pos, size_t len, std::string name, std::vector<TmClassifiedText>&& children = {})
			: pos(pos), len(len), name(std::move(name)), children(std::move(children))
		{
		}

		[[nodiscard]] size_t getEndPos() const noexcept
		{
			return pos + len;
		}
	};

	class TmParser : public StringParser
	{
	public:
		std::vector<TmClassifiedText> result{};

		using StringParser::StringParser;

		[[nodiscard]] const TmPattern* getNextMatch(const std::vector<TmPattern>& patterns) const;

		void processPatternList(const std::vector<TmPattern>& patterns)
		{
			processPatternList(patterns, result);
		}

		void processPatternList(const std::vector<TmPattern>& patterns, std::vector<TmClassifiedText>& result);

		/*void printClassifications() const
		{
			printClassifications(result, {});
		}

		void printClassifications(const std::vector<TmClassifiedText>& classifications, const std::string& prefix) const
		{
			for (const auto& c : classifications)
			{
				std::cout << prefix << c.name << ": '" << data.substr(c.pos, c.len) << "'\n";
				printClassifications(c.children, std::string(prefix).append("\t"));
			}
		}*/
	};
}
