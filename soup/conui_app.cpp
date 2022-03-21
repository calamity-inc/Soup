#include "conui_app.hpp"

#include "console.hpp"

namespace soup
{
	conui_app::conui_app()
		: conui_div(nullptr, 1, 1, -1, -1)
	{
		console.init();
		console.onMouseClick([](mouse_button b, unsigned int x, unsigned int y, const capture& cap)
		{
			//console << "You clicked at " << x << ", " << y << ". ";
			for (auto& child : cap.get<conui_app*>()->children)
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

	void conui_app::draw() const
	{
		console.clearScreen();
		conui_div::draw();
	}

	void conui_app::run()
	{
		draw();
		console.run();
	}
}
