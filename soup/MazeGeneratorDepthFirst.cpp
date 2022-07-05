#include "MazeGeneratorDepthFirst.hpp"

#include "RandomDevice.hpp"
#include "RenderTarget.hpp"
#include "Rgb.hpp"

namespace soup
{
	MazeGeneratorDepthFirst::MazeGeneratorDepthFirst(size_t width, size_t height)
		: width(width), height(height)
	{
		cells.resize(width * height);

		cells.at(0) = VISITED;
		backtrace.push(0);
	}

	bool MazeGeneratorDepthFirst::isFinished()
	{
		return backtrace.empty();
	}

	void MazeGeneratorDepthFirst::tick(RandomDevice& rd)
	{
		if (isFinished())
		{
			return;
		}

		std::vector<std::pair<uint8_t, size_t>> neighbours{};
		// Check up
		if (backtrace.top() >= width)
		{
			if (!(cells.at(backtrace.top() - width) & VISITED))
			{
				neighbours.emplace_back(CON_TOP, backtrace.top() - width);
			}
		}
		// Check left
		if (backtrace.top() % width != 0)
		{
			if (!(cells.at(backtrace.top() - 1) & VISITED))
			{
				neighbours.emplace_back(CON_LEFT, backtrace.top() - 1);
			}
		}
		// Check right
		if (((backtrace.top() + 1) % width) != 0)
		{
			if (!(cells.at(backtrace.top() + 1) & VISITED))
			{
				neighbours.emplace_back(CON_RIGHT, backtrace.top() + 1);
			}
		}
		// Check down
		if (backtrace.top() + width < cells.size())
		{
			if (!(cells.at(backtrace.top() + width) & VISITED))
			{
				neighbours.emplace_back(CON_DOWN, backtrace.top() + width);
			}
		}

		if (neighbours.empty())
		{
			backtrace.pop();
			return;
		}

		std::pair<uint8_t, size_t> neighbour = neighbours.at(rd.generate() % neighbours.size());
		cells.at(backtrace.top()) |= neighbour.first;
		cells.at(neighbour.second) |= VISITED;
		switch (neighbour.first)
		{
		case CON_TOP:
			cells.at(neighbour.second) |= CON_DOWN;
			break;

		case CON_LEFT:
			cells.at(neighbour.second) |= CON_RIGHT;
			break;

		case CON_RIGHT:
			cells.at(neighbour.second) |= CON_LEFT;
			break;

		case CON_DOWN:
			cells.at(neighbour.second) |= CON_TOP;
			break;
		}
		backtrace.emplace(neighbour.second);
	}

	std::pair<size_t, size_t> MazeGeneratorDepthFirst::getRenderSize() const
	{
		return { (width * 3) - 1, (height * 3) - 1 };
	}

	void MazeGeneratorDepthFirst::render(RenderTarget& rt) const
	{
		rt.fill(Rgb::BLACK);

		size_t i = 0;
		size_t x = 0;
		size_t y = 0;
		for (const auto& cell : cells)
		{
			rt.drawRect(x * 3, y * 3, 2, 2, ((!backtrace.empty() && i == backtrace.top()) ? Rgb::GREEN : Rgb::WHITE));
			if (cell & CON_RIGHT)
			{
				rt.drawRect((x * 3) + 2, y * 3, 1, 2, Rgb::WHITE);
			}
			if (cell & CON_DOWN)
			{
				rt.drawRect(x * 3, (y * 3) + 2, 2, 1, Rgb::WHITE);
			}

			++i;
			if (++x == width)
			{
				x = 0;
				++y;
			}
		}
	}
}
