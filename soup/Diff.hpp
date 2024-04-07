#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "fwd.hpp"

NAMESPACE_SOUP
{
	struct Diff
	{
		struct Indentation
		{
			bool spaces = false;
			unsigned int num = 0;

			[[nodiscard]] bool operator==(const Indentation& b) const noexcept
			{
				return spaces == b.spaces
					&& num == b.num
					;
			}

			[[nodiscard]] bool operator!=(const Indentation& b) const noexcept
			{
				return !operator==(b);
			}

			[[nodiscard]] std::string toString() const noexcept;
		};

		enum LineEnding : uint8_t
		{
			LF,
			CRLF,
			EOC, // for last line without extra line break
		};

		struct Line
		{
			std::string contents;
			Indentation indentation;
			LineEnding ending = LF;

			[[nodiscard]] bool isEmpty() const noexcept
			{
				return contents.empty()
					&& indentation.num == 0
					;
			}

			[[nodiscard]] std::string endingToString() const;

			[[nodiscard]] FormattedText toText() const;
			void toText(FormattedText& ft) const;
		};

		std::vector<std::optional<Diff::Line>> l, r;

		[[nodiscard]] static std::vector<Line> dissectLines(const std::string& data);

		[[nodiscard]] static Diff compute(const std::string& l, const std::string& r);
		[[nodiscard]] static Diff compute(const std::vector<Line>& l, const std::vector<Line>& r);

		static void highlightModifiedLine(FormattedText& ft, const Line& l, const Line& r);

		[[nodiscard]] FormattedText toText() const;
	};
}
