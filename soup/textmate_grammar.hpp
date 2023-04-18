#pragma once

#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace soup
{
	struct TmPattern
	{
		std::string name;
		std::regex match;
		std::vector<std::string> captures{};
		std::optional<std::regex> end_match;
		std::vector<std::string> end_captures{};
		std::vector<TmPattern> patterns;

		mutable std::smatch res;
		bool is_end = false;

		TmPattern(std::string name, std::regex match, std::vector<std::string> captures = {})
			: name(std::move(name)), match(std::move(match)), captures(std::move(captures))
		{
		}

		TmPattern(std::string name, std::regex begin_match, std::regex end_match, std::vector<TmPattern> patterns = {})
			: name(std::move(name)), match(std::move(begin_match)), end_match(std::move(end_match)), patterns(std::move(patterns))
		{
		}
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

	class TmParser
	{
	public:
		std::string code;
		std::string::const_iterator it;

		std::vector<TmClassifiedText> result{};

		TmParser(std::string code)
			: code(std::move(code)), it(this->code.cbegin())
		{
		}

		[[nodiscard]] const TmPattern* getNextMatch(const std::vector<TmPattern>& patterns) const;

		[[nodiscard]] size_t getPosition() const noexcept
		{
			return getPosition(it);
		}

		[[nodiscard]] size_t getPosition(std::string::const_iterator it) const noexcept
		{
			return it - code.cbegin();
		}

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
				std::cout << prefix << c.name << ": '" << code.substr(c.pos, c.len) << "'\n";
				printClassifications(c.children, std::string(prefix).append("\t"));
			}
		}*/
	};
}
