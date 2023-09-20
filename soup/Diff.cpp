#include "Diff.hpp"

#include "FormattedText.hpp"
#include "string.hpp"

namespace soup
{
	using Line = Diff::Line;

	std::vector<Line> Diff::dissectLines(const std::string& data)
	{
		std::vector<Line> res{};
		auto rawlines = string::explode(data, '\n');
		if (!rawlines.empty())
		{
			for (const auto& rawline : rawlines)
			{
				Line& line = res.emplace_back(Line{});
				line.contents = rawline;
				if (line.contents.empty())
				{
					continue;
				}
				if (line.contents.at(0) == '\t')
				{
					do
					{
						++line.indentation.num;
						line.contents.erase(0, 1);
					} while (!line.contents.empty()
						&& line.contents.at(0) == '\t'
						);
				}
				else if (line.contents.at(0) == ' ')
				{
					line.indentation.spaces = true;
					do
					{
						++line.indentation.num;
						line.contents.erase(0, 1);
					} while (!line.contents.empty()
						&& line.contents.at(0) == ' '
						);
				}
				if (line.contents.empty())
				{
					continue;
				}
				if (line.contents.back() == '\r')
				{
					line.ending = CRLF;
					line.contents.pop_back();
					if (line.contents.empty())
					{
						continue;
					}
				}
			}
			if (res.back().isEmpty())
			{
				res.pop_back();
			}
			else
			{
				if (res.back().ending == CRLF)
				{
					res.back().contents.push_back('\r');
				}
				res.back().ending = EOC;
			}
		}
		return res;
	}

	Diff Diff::compute(const std::string& l, const std::string& r)
	{
		return compute(dissectLines(l), dissectLines(r));
	}

	Diff Diff::compute(const std::vector<Line>& l, const std::vector<Line>& r)
	{
		Diff d;

		size_t i = 0;
		for (; i != l.size(); ++i)
		{
			if (i >= r.size())
			{
				d.l.emplace_back(l[i]);
				d.r.emplace_back(std::nullopt);
			}
			else if (l[i].contents == r[i].contents)
			{
				d.l.emplace_back(l[i]);
				d.r.emplace_back(r[i]);
			}
			else
			{
				d.l.emplace_back(l[i]);
				d.l.emplace_back(std::nullopt);
				d.r.emplace_back(std::nullopt);
				d.r.emplace_back(r[i]);
			}
		}
		if (i < r.size())
		{
			for (; i != r.size(); ++i)
			{
				d.l.emplace_back(std::nullopt);
				d.r.emplace_back(r[i]);
			}
		}

		return d;
	}

	void Diff::highlightModifiedLine(FormattedText& ft, const Line& l, const Line& r)
	{
		if (l.indentation != r.indentation)
		{
			ft.addSpan(l.indentation.toString(), Rgb::GREY, Rgb::RED);
			ft.addSpan(r.indentation.toString(), Rgb::GREY, Rgb::GREEN);
		}
		else
		{
			ft.addSpan(l.indentation.toString(), Rgb::GREY);
		}

		if (l.contents != r.contents)
		{
			ft.addSpan(l.contents, Rgb::WHITE, Rgb::RED);
			ft.addSpan(r.contents, Rgb::WHITE, Rgb::GREEN);
		}
		else
		{
			ft.addSpan(l.contents, Rgb::WHITE);
		}

		if (l.ending != r.ending)
		{
			ft.addSpan(l.endingToString(), Rgb::GREY, Rgb::RED);
			ft.addSpan(r.endingToString(), Rgb::GREY, Rgb::GREEN);
		}
		else
		{
			ft.addSpan(l.endingToString(), Rgb::GREY);
		}
	}

	std::string Diff::Indentation::toString() const noexcept
	{
		if (spaces)
		{
			return std::string(num, '.');
		}
		std::string str;
		for (auto i = num; i != 0; --i)
		{
			str.append("----");
		}
		return str;
	}

	std::string Diff::Line::endingToString() const
	{
		switch (ending)
		{
		case LF: return "LF";
		case CRLF: return "CRLF";
		default: case EOC: return "EOC";
		}
	}

	FormattedText Diff::Line::toText() const
	{
		FormattedText ft;
		toText(ft);
		return ft;
	}

	void Diff::Line::toText(FormattedText& ft) const
	{
		ft.addSpan(indentation.toString(), Rgb::GREY);
		ft.addSpan(contents, Rgb::WHITE);
		ft.addSpan(endingToString(), Rgb::GREY);
	}

	FormattedText Diff::toText() const
	{
		FormattedText ft;
		for (auto i = 0; i != l.size(); ++i)
		{
			if (l[i].has_value() && r[i].has_value())
			{
				Diff::highlightModifiedLine(ft, *l[i], *r[i]);
			}
			else
			{
				if (l[i].has_value())
				{
					l[i]->toText(ft);
					for (auto& span : ft.lines.back())
					{
						span.bg = Rgb::RED;
					}
				}
				else
				{
					r[i]->toText(ft);
					for (auto& span : ft.lines.back())
					{
						span.bg = Rgb::GREEN;
					}
				}
			}
			ft.newLine();
		}
		return ft;
	}
}
