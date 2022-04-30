#include "ConuiApp.hpp"

#include "console.hpp"

namespace soup
{
	ConuiApp::ConuiApp()
		: ConuiDiv(nullptr, 1, 1, -1, -1)
	{
		console.init(true);
		console.onMouseClick([](MouseButton b, unsigned int x, unsigned int y, const Capture& cap)
		{
			//console << "You clicked at " << x << ", " << y << ". ";
			for (auto& child : cap.get<ConuiApp*>()->children)
			{
				//console << "Child at " << child->x << ", " << child->y << " has dimensions of " << child->width << ", " << child->height << ". ";
				if (x >= child->x
					&& y >= child->y
					&& x < (child->x + child->width)
					&& y < (child->y + child->height)
					)
				{
					child->onClick(b, x, y);
				}
			}
		}, this);
	}

	void ConuiApp::draw() const
	{
		console.clearScreen();
		ConuiDiv::draw();
	}

	void ConuiApp::run()
	{
		draw();
		console.run();
	}
}
