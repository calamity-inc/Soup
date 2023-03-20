#include "Svg.hpp"

#include "base.hpp"

#include "joaat.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"
#include "string.hpp"
#include "Vector2.hpp"
#include "visCurves.hpp"
#include "xml.hpp"

namespace soup
{
	Svg::Svg(const XmlTag& data)
		: data(data)
	{
		if (data.hasAttribute("width"))
		{
			width = std::stoi(data.getAttribute("width"));
			height = std::stoi(data.getAttribute("height"));
		}
		else if (data.hasAttribute("viewBox"))
		{
			auto arr = string::explode(data.getAttribute("viewBox"), ' ');
			width = std::stoi(arr.at(2));
			height = std::stoi(arr.at(3));
		}
	}

	float Svg::translateX(const RenderTarget& rt, float x) const noexcept
	{
		return x / width * rt.width;
	}

	float Svg::translateY(const RenderTarget& rt, float y) const noexcept
	{
		return y / height * rt.height;
	}

	Vector2 Svg::translate(const RenderTarget& rt, const Vector2& v) const noexcept
	{
		return Vector2(
			translateX(rt, v.x),
			translateY(rt, v.y)
		);
	}

	void Svg::execute(RenderTarget& rt) const
	{
		for (const auto& _node : data.children)
		{
			if (_node->is_text)
			{
				continue;
			}
			const XmlTag& node = reinterpret_cast<const XmlTag&>(*_node);
			switch (joaat::hash(node.name))
			{
			case joaat::hash("line"):
				// Note: stroke-width is currently ignored.
				// - stroke-width should be defaulted to 1.
				// - X & Y should have the same scaling.
				// - stroke-width should be scaled with X & Y.
				// - Same stroke-width handling may be needed for <path>.
				rt.drawLine(
					Vector2(
						translateX(rt, std::stof(node.getAttribute("x1"))),
						translateY(rt, std::stof(node.getAttribute("y1")))
					),
					Vector2(
						translateX(rt, std::stof(node.getAttribute("x2"))),
						translateY(rt, std::stof(node.getAttribute("y2")))
					),
					Rgb::fromHex(node.getAttribute("stroke"))
				);
				break;

			case joaat::hash("circle"):
				rt.drawEllipse(
					translateX(rt, std::stof(node.getAttribute("cx"))),
					translateY(rt, std::stof(node.getAttribute("cy"))),
					translateX(rt, std::stof(node.getAttribute("r"))),
					translateY(rt, std::stof(node.getAttribute("r"))),
					Rgb::fromHex(node.getAttribute("fill"))
				);
				break;

			case joaat::hash("ellipse"):
				rt.drawEllipse(
					translateX(rt, std::stof(node.getAttribute("cx"))),
					translateY(rt, std::stof(node.getAttribute("cy"))),
					translateX(rt, std::stof(node.getAttribute("rx"))),
					translateY(rt, std::stof(node.getAttribute("ry"))),
					Rgb::fromHex(node.getAttribute("fill"))
				);
				break;

			case joaat::hash("path"):
				// Note: stroke is currently ignored.
				// - fill could also be "transparent" for stroke only.
				executePath(rt, node.getAttribute("d"), Rgb::fromHex(node.getAttribute("fill")));
				break;
			}
		}
	}

#define INVARG (-FLT_MAX)

	struct SvgPathState
	{
		const Svg& svg;
		const RenderTarget& rt;
		std::vector<Vector2> path;

		float args[6];

		Vector2 cursor{ 0, 0 };
		Vector2 first_point;
		Vector2 last_cubic_control_point;

		SvgPathState(const Svg& svg, const RenderTarget& rt)
			: svg(svg), rt(rt)
		{
			resetArgs();
			first_point.x = -1;
			last_cubic_control_point.x = -1;
		}

		void resetArgs() noexcept
		{
			for (auto& arg : args)
			{
				arg = INVARG;
			}
		}

		void pushArg(float i) noexcept
		{
			for (auto& arg : args)
			{
				if (arg == INVARG)
				{
					arg = i;
					break;
				}
			}
		}
		
		[[nodiscard]] unsigned int getNumArgs() const noexcept
		{
			unsigned int tally = 0;
			for (const auto& arg : args)
			{
				if (arg == INVARG)
				{
					break;
				}
				++tally;
			}
			return tally;
		}

		[[nodiscard]] static unsigned int getNeededArgs(char cmd) noexcept
		{
			switch (cmd)
			{
			case 'Z': return 0;
			case 'V': case 'v': return 1;
			case 'H': case 'h': return 1;
			case 'C': case 'c': return 6;
			case 'S': case 's': return 4;
			}
			return 2;
		}

		void setCursor(Vector2 v) noexcept
		{
			cursor = v;
			if (first_point.x == -1)
			{
				first_point = cursor;
			}
		}

		void addPoint(Vector2 v)
		{
			path.emplace_back(translate(v));
		}

		void addPointAndSetCursor(Vector2 v)
		{
			addPoint(v);
			setCursor(v);
		}

		[[nodiscard]] Vector2 translate(const Vector2& v) const
		{
			return svg.translate(rt, v);
		}

		void executeCmd(char cmd)
		{
			switch (cmd)
			{
				case 'M': {
					SOUP_ASSERT(getNumArgs() == 2);
					setCursor(Vector2(args[0], args[1]));
				}
				break;

				case 'm': {
					SOUP_ASSERT(getNumArgs() == 2);
					setCursor(cursor + Vector2(args[0], args[1]));
				}
				break;

				case 'L': {
					SOUP_ASSERT(getNumArgs() == 2);
					addPointAndSetCursor(Vector2(args[0], args[1]));
				}
				break;

				case 'l': {
					SOUP_ASSERT(getNumArgs() == 2);
					addPointAndSetCursor(cursor + Vector2(args[0], args[1]));
				}
				break;

				case 'Z':
				case 'z': {
					SOUP_ASSERT(getNumArgs() == 0);
					addPointAndSetCursor(first_point);
				}
				break;

				case 'H': {
					SOUP_ASSERT(getNumArgs() == 1);
					auto dest = cursor;
					dest.x = args[0];
					addPointAndSetCursor(dest);
				}
				break;

				case 'h': {
					SOUP_ASSERT(getNumArgs() == 1);
					auto dest = cursor;
					dest.x += args[0];
					addPointAndSetCursor(dest);
				}
				break;

				case 'V': {
					SOUP_ASSERT(getNumArgs() == 1);
					auto dest = cursor;
					dest.y = args[0];
					addPointAndSetCursor(dest);
				}
				break;

				case 'v': {
					SOUP_ASSERT(getNumArgs() == 1);
					auto dest = cursor;
					dest.y += args[0];
					addPointAndSetCursor(dest);
				}
				break;

				case 'C': {
					SOUP_ASSERT(getNumArgs() == 6);
					//std::cout << "C " << args[0] << ", " << args[1] << ", " << args[2] << ", " << args[3] << ", " << args[4] << ", " << args[5] << "\n";
					Vector2 a(args[0], args[1]);
					Vector2 b(args[2], args[3]);
					Vector2 c(args[4], args[5]);
					for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(c)))
					{
						addPoint(visCurves::cubicBezier(cursor, a, b, c, t));
					}
					setCursor(c);
					last_cubic_control_point = b;
				}
				break;

				case 'c': {
					SOUP_ASSERT(getNumArgs() == 6);
					auto a = cursor + Vector2(args[0], args[1]);
					auto b = cursor + Vector2(args[2], args[3]);
					auto c = cursor + Vector2(args[4], args[5]);
					for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(c)))
					{
						addPoint(visCurves::cubicBezier(cursor, a, b, c, t));
					}
					setCursor(c);
					last_cubic_control_point = b;
				}
				break;

				case 'S': {
					SOUP_ASSERT(getNumArgs() == 4);
					Vector2 a = cursor;
					if (last_cubic_control_point.x != -1)
					{
						a += (cursor - last_cubic_control_point);
					}
					Vector2 b(args[0], args[1]);
					Vector2 c(args[2], args[3]);
					for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(c)))
					{
						addPoint(visCurves::cubicBezier(cursor, a, b, c, t));
					}
					setCursor(c);
					last_cubic_control_point = b;
				}
				break;

				case 's': {
					SOUP_ASSERT(getNumArgs() == 4);
					Vector2 a = cursor;
					if (last_cubic_control_point.x != -1)
					{
						a += (cursor - last_cubic_control_point);
					}
					auto b = cursor + Vector2(args[0], args[1]);
					auto c = cursor + Vector2(args[2], args[3]);
					for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(c)))
					{
						addPoint(visCurves::cubicBezier(cursor, a, b, c, t));
					}
					setCursor(c);
					last_cubic_control_point = b;
				}
				break;
			}
		}

		// https://en.wikipedia.org/wiki/Even%E2%80%93odd_rule
		[[nodiscard]] bool isPointInPath(float x, float y) noexcept
		{
			size_t num = path.size();
			size_t j = num - 1;
			bool c = false;
			for (size_t i = 0; i != num; ++i)
			{
				if ((x == path[i].x) && (y == path[i].y))
				{
					// point is a corner
					return true;
				}
				if ((path[i].y > y) != (path[j].y > y))
				{
					auto slope = (x-path[i].x)*(path[j].y-path[i].y)-(path[j].x-path[i].x)*(y-path[i].y);
					if (slope == 0)
					{
						// point is on boundary
						return true;
					}
					if ((slope < 0) != (path[j].y < path[i].y))
					{
						c = !c;
					}
				}
				j = i;
			}
			return c;
		}
	};

	void Svg::executePath(RenderTarget& rt, const std::string& line, Rgb fill) const
	{
		SvgPathState state(*this, rt);
		char cmd = 0;
		std::string argbuf{};
		for (const auto& c : line)
		{
			if (c == ' ' || c == ',')
			{
				if (!argbuf.empty())
				{
					state.pushArg(std::stof(argbuf));
					argbuf.clear();
				}
				continue;
			}
			if (string::isNumberChar(c)
				|| c == '-'
				|| c == '.'
				)
			{
				if (c == '-')
				{
					if (!argbuf.empty())
					{
						state.pushArg(std::stof(argbuf));
						argbuf.clear();
					}
				}
				else if (c == '.')
				{
					if (argbuf.find('.') != std::string::npos)
					{
						state.pushArg(std::stof(argbuf));
						argbuf.clear();
					}
				}
				if (argbuf.empty())
				{
					if (cmd != 0)
					{
						if (state.getNumArgs() == state.getNeededArgs(cmd))
						{
							state.executeCmd(cmd);
							state.resetArgs();
						}
					}
				}
				argbuf.push_back(c);
				continue;
			}

			if (cmd != 0)
			{
				if (!argbuf.empty())
				{
					state.pushArg(std::stof(argbuf));
					argbuf.clear();
				}
				state.executeCmd(cmd);
				state.resetArgs();
			}
			cmd = c;
		}
		if (cmd != 0)
		{
			if (!argbuf.empty())
			{
				state.pushArg(std::stof(argbuf));
			}
			state.executeCmd(cmd);
		}

		// Draw filled shape
		for (unsigned int y = 0; y != rt.height; ++y)
		{
			for (unsigned int x = 0; x != rt.width; ++x)
			{
				if (state.isPointInPath(x, y))
				{
					rt.drawPixel(x, y, fill);
				}
			}
		}
	}
}
