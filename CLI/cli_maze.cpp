#include "cli.hpp"

#include <thread>

#include <Canvas.hpp>
#include <console.hpp>
#include <algLcgRng.hpp>
#include <MazeGeneratorDepthFirst.hpp>
#include <RenderTargetCanvas.hpp>

using namespace soup;

void cli_maze()
{
	console.init(true);
	console.enableSizeTracking([](unsigned int width, unsigned int height, const Capture&)
	{
		static bool once = false;
		if (once)
		{
			return;
		}
		once = true;
		algLcgRng rng{};
		height *= 2;
		height -= 3;
		MazeGeneratorDepthFirst mg((width + 1) / 3, (height + 1) / 3);
		for (bool last = true; (mg.isFinished() ? (last ? (last = false, true) : false) : true); mg.tick(rng))
		{
			Canvas c(width, height);
			RenderTargetCanvas rt(c);
			mg.render(rt);
			console.clearScreen();
			console << c.toStringDownsampledDoublewidth(true);
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	});
	console.overrideCtrlC([]
	{
		console.cleanup();
		exit(0);
	});
	console.run();
}
