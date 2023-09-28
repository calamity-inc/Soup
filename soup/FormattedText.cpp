#include "FormattedText.hpp"

#include "console.hpp"
#include "RenderTarget.hpp"
#include "RasterFont.hpp"

namespace soup
{
	void FormattedText::addSpan(std::string text)
	{
		addSpan(Span{ std::move(text) });
	}

	void FormattedText::addSpan(std::string text, Rgb fg)
	{
		addSpan(Span{ std::move(text), std::move(fg) });
	}

	void FormattedText::addSpan(std::string text, Rgb fg, Rgb bg)
	{
		addSpan(Span{ std::move(text), std::move(fg), std::move(bg) });
	}

	void FormattedText::addSpan(Span&& span)
	{
		if (lines.empty())
		{
			newLine();
		}
		lines.back().emplace_back(std::move(span));
	}

	void FormattedText::newLine()
	{
		lines.emplace_back(std::vector<Span>{});
	}

	std::pair<size_t, size_t> FormattedText::measure(const RasterFont& font) const
	{
		size_t width = 0;
		size_t height = 0;
		for (const auto& line : lines)
		{
			for (const auto& span : line)
			{
				width += font.measureWidth(span.text);
			}
			height += font.baseline_glyph_height;
		}
		return { width, height + 1 }; // heuristic to handle glyphs like ',' going beyond height
	}

	void FormattedText::draw(RenderTarget& rt, const RasterFont& font) const
	{
		size_t x = 0;
		size_t y = 0;
		for (const auto& line : lines)
		{
			for (const auto& span : line)
			{
				rt.drawText(x, y, span.text, font, span.fg.rgb);
				x += font.measureWidth(span.text);
			}
			y += font.baseline_glyph_height;
		}
	}

	std::string FormattedText::toString() const
	{
		std::string str{};
		for (auto line = lines.begin();; )
		{
			for (const auto& span : *line)
			{
				if (span.fg.reset || span.bg.reset)
				{
					str.append(console.strResetColour<std::string>());
				}
				if (!span.fg.reset)
				{
					str.append(console.strSetForegroundColour<std::string>(span.fg.rgb.r, span.fg.rgb.g, span.fg.rgb.b));
				}
				if (!span.bg.reset)
				{
					str.append(console.strSetBackgroundColour<std::string>(span.bg.rgb.r, span.bg.rgb.g, span.bg.rgb.b));
				}
				str.append(span.text);
			}
			str.append(console.strResetColour<std::string>());
			if (++line == lines.end())
			{
				break;
			}
			str.push_back('\n');
		}
		return str;
	}

	std::string FormattedText::toHtml() const
	{
		std::string str{};
		for (auto line = lines.begin();; )
		{
			for (const auto& span : *line)
			{
				std::string css{};
				if (!span.fg.reset)
				{
					css.append("color:");
					css.append(span.fg.rgb.toHex());
					css.push_back(';');
				}
				if (!span.bg.reset)
				{
					css.append("background-color:");
					css.append(span.bg.rgb.toHex());
					css.push_back(';');
				}
				if (!css.empty())
				{
					str.append(R"(<span style=")");
					str.append(css);
					str.append(R"(">)");
					str.append(span.text);
					str.append("</span>");
				}
				else
				{
					str.append(span.text);
				}
			}
			if (++line == lines.end())
			{
				break;
			}
			str.append("<br>");
		}
		return str;
	}
}
