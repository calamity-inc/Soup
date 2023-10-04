#include "cli.hpp"

#include <thread>

#include <Canvas.hpp>
#include <console.hpp>
#include <RasterFont.hpp>

using namespace soup;

static Canvas c{};
static int64_t pos_x = 1;
static int64_t pos_y = 1;
static int64_t vel_x = 1;
static int64_t vel_y = 1;
static bool started_loop = false;

void cli_dvd()
{
	console.init(true);
	console.enableSizeTracking([](unsigned int width, unsigned int height, const Capture&)
	{
		c.resize(width, height * 2);

		if (!started_loop)
		{
			started_loop = true;
			std::thread t([]
			{
				while (true)
				{
					static auto font = RasterFont::simple8();
					c.fill(Rgb::BLACK);
					pos_x += vel_x;
					pos_y += vel_y;
					if (pos_x <= 0)
					{
						vel_x = 1;
					}
					if (pos_y <= 0)
					{
						vel_y = 1;
					}
					if (pos_x >= c.width - font.measureWidth("SOUP"))
					{
						pos_x = c.width - font.measureWidth("SOUP");
						vel_x = -1;
					}
					if (pos_y >= c.height - font.baseline_glyph_height)
					{
						pos_y = c.height - font.baseline_glyph_height;
						vel_y = -1;
					}
					c.addText(pos_x, pos_y, "SOUP", font);
					console.setCursorPos(0, 0);
					console << c.toStringDownsampledDoublewidth(false);
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			});
			t.detach();
		}
	});
	console.run();
}
