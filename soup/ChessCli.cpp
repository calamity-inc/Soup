#include "ChessCli.hpp"

#include "console.hpp"
#include "MouseButton.hpp"

namespace soup
{
	void ChessCli::run()
	{
		console.init();
		if (dark_theme)
		{
			console.setBackgroundColour(0, 0, 0);
			console.setForegroundColour(255, 255, 255);
		}
		else
		{
			console.setBackgroundColour(255, 255, 255);
			console.setForegroundColour(0, 0, 0);
		}
		console << board.toString(dark_theme);

		console.onMouseClick([](MouseButton b, unsigned int x, unsigned int y, const Capture& cap)
		{
			if (b != LMB)
			{
				return;
			}
			auto coord = console2chess(x, y);
			if (!coord.isValid())
			{
				return;
			}

			auto& cc = *cap.get<ChessCli*>();

			if (!cc.selection.isValid())
			{
				cc.selection = coord;
			}
			else
			{
				if (cc.selection == coord)
				{
					cc.selection.invalidate();
				}
				else
				{
					if (cc.board.square(coord).isEmpty())
					{
						cc.selection = coord;
					}
					else
					{
						cc.board.square(coord) = cc.board.square(cc.selection);
						cc.board.square(cc.selection).reset();
						cc.selection.invalidate();
					}
				}
			}

			console.clearScreen();
			console.setCursorPos(0, 0);
			console << cc.board.toString(cc.dark_theme);
			if (cc.selection.isValid())
			{
				console.setCursorPos(x, y);
				console.setForegroundColour(255, 0, 255);
				console << cc.board.toString(coord, cc.dark_theme);
				console.setForegroundColour(255, 255, 255);

				for (const auto& move : cc.board.square(coord).getMoves(coord))
				{
					auto xy = chess2console(move);
					console.setCursorPos(xy.first, xy.second);
					console.setBackgroundColour(255, 0, 255);
					console << cc.board.toString(move, cc.dark_theme);
				}
				console.setBackgroundColour(0, 0, 0);
			}
			console.setCursorPos(0, 8);
		}, this);

		console.overrideCtrlC([]
		{
			console.cleanup();
			exit(0);
		});

		console.run();
	}
}
