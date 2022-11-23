#pragma once

#include "fwd.hpp"

#include <string>
#include <utility>
#include <vector>

#include "Rgb.hpp"

namespace soup
{
	struct FormattedText
	{
		struct Span
		{
			std::string text;
			Rgb colour;
		};

		std::vector<std::vector<Span>> lines{};

		void addSpan(std::string text, Rgb colour);

		[[nodiscard]] std::pair<size_t, size_t> measure(const RasterFont& font) const;

		void draw(RenderTarget& rt, const RasterFont& font) const;

		[[nodiscard]] std::string toString() const;
	};
}
