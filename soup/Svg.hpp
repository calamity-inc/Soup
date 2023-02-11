#pragma once

#include <string>

#include "fwd.hpp"

namespace soup
{
	// This has been tested against the following two images:
	// - https://abs-0.twimg.com/emoji/v2/svg/1f600.svg
	// - https://abs-0.twimg.com/emoji/v2/svg/1f642.svg
	// Anything else may very well blow up at this point.
	class Svg
	{
	public:
		const XmlTag& data;
		unsigned int width, height;

		Svg(const XmlTag& data);

		[[nodiscard]] float translateX(const RenderTarget& rt, float x) const noexcept;
		[[nodiscard]] float translateY(const RenderTarget& rt, float y) const noexcept;
		[[nodiscard]] Vector2 translate(const RenderTarget& rt, const Vector2& v) const noexcept;

		void execute(RenderTarget& rt) const;
	private:
		void executePath(RenderTarget& rt, const std::string& line, Rgb fill) const;
	};
}
