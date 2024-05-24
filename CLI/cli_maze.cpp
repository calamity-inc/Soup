#include "cli.hpp"

#include <Canvas.hpp>
#include <console.hpp>
#include <LcgRngInterface.hpp>
#include <MazeGeneratorDepthFirst.hpp>
#include <os.hpp>
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
		LcgRngInterface rng{};
		height *= 2;
		height -= 3;
		MazeGeneratorDepthFirst mg((width + 1) / 3, (height + 1) / 3);
		for (bool last = true; (mg.isFinished() ? (last ? (last = false, true) : false) : true); mg.tick(rng))
		{
			Canvas c(width, height);
			RenderTargetCanvas rt(c);
			mg.render(rt);
			console.clearScreen();
			console << c.toStringDownsampledDoublewidth(true, true);
			os::sleep(200);
		}
	});
	console.overrideCtrlC([]
	{
		console.cleanup();
		exit(0);
	});
	console.run();
}
